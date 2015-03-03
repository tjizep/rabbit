.PHONY: clean All

All:
	@echo "----------Building project:[ rabbit - Release ]----------"
	@$(MAKE) -f  "rabbit.mk"
clean:
	@echo "----------Cleaning project:[ rabbit - Release ]----------"
	@$(MAKE) -f  "rabbit.mk" clean
