#配置
TAG := ./base 
MAKE = make

#执行内容
all: $(TAG)
$(TAG): ECHO
	$(MAKE) -C $@

ECHO:
	@echo $(TAG)

#命令
.PHONY: clean

clean: 
	$(MAKE) clean -C $(TAG)
