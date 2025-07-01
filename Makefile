.PHONY: clean All

All:
	@echo "----------Building project:[ Client_Stream - Debug ]----------"
	@cd "Client_Stream" && "$(MAKE)" -f  "Client_Stream.mk"
clean:
	@echo "----------Cleaning project:[ Client_Stream - Debug ]----------"
	@cd "Client_Stream" && "$(MAKE)" -f  "Client_Stream.mk" clean
