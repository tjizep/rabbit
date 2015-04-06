.PHONY: clean All

All:
	@echo "----------Building project:[ cache_test - Release ]----------"
	@cd "cache_test" && $(MAKE) -f  "cache_test.mk"
clean:
	@echo "----------Cleaning project:[ cache_test - Release ]----------"
	@cd "cache_test" && $(MAKE) -f  "cache_test.mk" clean
