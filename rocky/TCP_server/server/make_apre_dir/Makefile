SAMPLE_NAME = select

ALTI_INCLUDE=${ALTIBASE_HOME}/include
ALTI_LIBRARY=${ALTIBASE_HOME}/lib -lapre -lodbccli -lpthread -lm -ldl -lrt

ALL : ${SAMPLE_NAME}.c ${SAMPLE_NAME}

${SAMPLE_NAME}.c:./sample/APRE/${SAMPLE_NAME}.sc
	apre -o . -t c ./sample/APRE/${SAMPLE_NAME}.sc 

${SAMPLE_NAME}:${SAMPLE_NAME}.c
	cc -o ${SAMPLE_NAME} ${SAMPLE_NAME}.c -I$(ALTI_INCLUDE) -L$(ALTI_LIBRARY)

fclean:
	rm ${SAMPLE_NAME}.c ${SAMPLE_NAME}
