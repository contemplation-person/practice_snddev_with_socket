ALTI_INCLUDE=$(ALTIBASE_HOME)/include
ALTI_LIBRARY=$(ALTIBASE_HOME)/lib -lapre -lodbccli -lpthread -lm -ldl

LFLAGS= -g3


APRE_TARGET=connect1

ALL:$(APRE_TARGET).c $(APRE_TARGET)

$(APRE_TARGET).c:sample/APRE/$(APRE_TARGET).sc
	./apre -t c $<
	mv sample/APRE/$(APRE_TARGET).c .

$(APRE_TARGET):$(APRE_TARGET).c
	gcc $(LFLAGS) -o $@ $< -I$(ALTI_INCLUDE) -L$(ALTI_LIBRARY) 

clean:
	rm $(APRE_TARGET) $(APRE_TARGET).c

