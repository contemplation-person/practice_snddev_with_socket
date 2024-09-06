SAMPLE_NAME = connect1

ALTI_INCLUDE=${ALTIBASE_HOME}/include
ALTI_LIBRARY=${ALTIBASE_HOME}/lib -lapre -lodbccli -lpthread -lm -ldl

ALL : ${SAMPLE_NAME}.c ${SAMPLE_NAME}

${SAMPLE_NAME}.c:./sample/APRE/${SAMPLE_NAME}.sc
	apre -t c ./sample/APRE/${SAMPLE_NAME}.sc
	mv ./sample/APRE/${SAMPLE_NAME}.c .

${SAMPLE_NAME}:${SAMPLE_NAME}.c
	cc -o ${SAMPLE_NAME} ${SAMPLE_NAME}.c -I$(ALTI_INCLUDE) -L$(ALTI_LIBRARY)

fclean:
	rm ${SAMPLE_NAME}.c ${SAMPLE_NAME}
