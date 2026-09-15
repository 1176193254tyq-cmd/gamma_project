/* pf_convert_final.c
 * (P, PF) ←→ (Q, S) 转换 & 设备口径 (S, pf) 指令生成（PF 与 Q ）
 *  						    LC									                       台达
 *  						   Q+									                        Q+
 *  						    ↑									                          ↑
 *   					    │										                   │
 *	 	    Q2 (P-,Q+,pf+)  │    Q1 (P+,Q+,pf+)                           Q2 (P-,Q+,S+,pf-)  │    Q1 (P+,Q+,S+,pf+)
 *    					    |								    		                          │
 *  						    |					  				                                │
 *       P-  -------------------+-----------------→ P+    ←转换→   P-  -------------------+-----------------→ P+
 *       					    |											                   │
 *  		 Q3 (P-,Q-,pf-)       │    Q4 (P+,Q-,pf-)	                       Q3 (P-,Q-,S-,pf-)   │    Q4 (P+,Q-,S+,pf+)
 *    					    |											                   │
 *    					    | 											             │
 *   					    ↓ Q-											             ↓ Q-
 * 规则：
 *      参数             第1象限(P+,Q+)    第2象限(P-Q+)      第3象限(P-Q-)    第4象限(P+Q-)    
 *   Lc接收到的设置P          正                         负                         负                       正
 *   Lc接收到的设置PF        正                         正                         负                        负
 *   S_cmd                        正                         正                          负                       负
 *   pf_cmd                       正                         负                          负                       正
 *   Lc需要上传的S_out       正                         正                         正                       正
 *   Lc需要上传的pf_out     正                         正                          负                       负
 *
 *Lc封装后：
 *   P > 0 表示放电/发电/向电网送有功；P < 0 表示充电/负载/从电网吸收有功。
 *   Q > 0 表示过励磁、升压、容性方向；Q < 0 表示欠励磁、降压、感性方向。
 *   PF 的大小决定 |cos φ| 的大小，PF 与 Q 的符号方向一致，PF > 0 表示过励磁、leading电流超前电压；PF < 0 表示欠励磁 、lagging电流滞后电压。
 
 */
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "pf_convert_final.h"
/* ---------- 可按现场调参的常量 ---------- */
#define PF_MIN     0.01f   /* |PF| 下限，避免除零；如站侧要求 0.8/0.95 可改大 */
#define PF_EPS     1e-4f   /* 与 1 的间隙，避免 sqrt(1-pf^2) 数值抖动 */
#define P_EPS      1e-6f   /* 认为 P≈0 的阈值（PF 模式下 P=0 无定义 */
																
/* ---------- 工具函数 ---------- */
 float f_abs(float x) { return x >= 0.0f ? x : -x; }
 int   f_sgn(float x) { return (x > 0.0f) - (x < 0.0f); }
 float f_clamp(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

/* ---------- 1) (P, PF)（Lc接收的指令）-> (S_cmd, pf_cmd)（设备口径） ----------
 * 设备内部按 P=|S|*pf、Q=S*sqrt(1-pf^2) 计算，可得到与期望一致的 P/Q
 */
bool P_PF_to_S_pf_cmd(float P, float PF, float* S_cmd, float* pf_cmd)
{
    if (!S_cmd || !pf_cmd) return false;

    float pf_abs  = f_clamp(f_abs(PF), PF_MIN, 1.0f - PF_EPS);
    float S_abs = f_abs(P) / pf_abs;
    float Q_abs = sqrtf((S_abs * S_abs) - (P * P));
    if (!within_apparent_limit(P, Q_abs, 2782))
    {
        return false ;
    }
    int   sS = 1;
    int   spf = 1;
    if((P>=0.0f)&&(PF >= 0.0f))   /* 第1象限(P+,Q+) */
    {
       sS = 1;
       spf = 1;
    }
    else if((P<0.0f)&&(PF >= 0.0f)) /* 第2象限(P-,Q+)*/
    {
       sS =  1;
       spf = -1;
    }
    else if((P<0.0f)&&(PF < 0.0f))  /* 第3象限(P-,Q-) */
    {
       sS =  -1;
       spf = -1;
    }
    else if((P>=0.0f)&&(PF < 0.0f)) /* 第4象限(P+,Q-) */
    {
       sS =  -1;
       spf = 1;
    }
    *S_cmd  = (float)sS * S_abs;
    *pf_cmd = (float)spf *pf_abs;
    return true;
}

/* ---------- 2) (P_read, Q_read, PF_read)（Lc从PCS回读的）-> (S_out, pf_out)（Lc上传ems） ----------
 */
void P_PF_read_to_S_pf_out(float P_read, float Q_read, float PF_read, float* S_out, float* pf_out)
{
    if (!S_out || !pf_out) return;

    float pf_abs = f_abs(PF_read);

    if (pf_abs < PF_MIN)
    {
        pf_abs = PF_MIN;
    }
    else if (pf_abs > 1.0f)
    {
        pf_abs = 1.0f;
    }

    float S_abs = sqrtf((P_read * P_read) + (Q_read * Q_read));

    int spf = 1;

    if ((P_read >= 0.0f) && (Q_read >= 0.0f))
    {
        spf = 1;
    }
    else if ((P_read < 0.0f) && (Q_read >= 0.0f))
    {
        spf = 1;
    }
    else if ((P_read < 0.0f) && (Q_read < 0.0f))
    {
        spf = -1;
    }
    else if ((P_read >= 0.0f) && (Q_read < 0.0f))
    {
        spf = -1;
    }

    *S_out = S_abs;
    *pf_out = (float)spf * pf_abs;
}
/* ---------- 3) 椭圆限幅校核 ----------
 * 校核：P^2 + Q^2 <= Smax^2
 */
 bool within_apparent_limit(float P, float Q, float Smax)
{
    return (P*P + Q*Q) <= (Smax*Smax + 1e-6f);
}

          