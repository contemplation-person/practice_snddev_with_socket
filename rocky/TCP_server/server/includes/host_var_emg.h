EXEC SQL BEGIN DECLARE SECTION;

typedef struct {
    char LTEID[21];       // 기업 ID
    char SLICE_ID[5];     // 슬라이스 ID
    char DEVICE_ID[18];   // 2차 단말의 DEVICE_ID
    char MDN[12];         // 1차 단말의 MDN
    char IP[40];          // 사용 IP
    char USER_ID[23];     // 사용자 ID
    char DEVICE_TYPE[11]; // 단말 타입 (PC, CCTV, PRINTER, ETC)
    char MODEL_NAME[31];  // 모델명
    char SERIAL_NUM[31];  // 시리얼 번호

    int AUTH_TYPE;        // 인증 타입 (0: EAP 인증, 1: EAP 미인증)
    int TWO_FA_USE;       // 2-factor 인증 사용 여부
    int DEVICE_SUSPEND;   // 단말 일시정지 여부
    int ID_TYPE;          // ID 타입 (1: MAC, 2: CPU serial, 3: 추후 정의)
    int IP_POOL_INDEX;    // IP Pool Index
} Emg_type;

EXEC SQL END DECLARE SECTION;

