/**
 * @file   restApiMessage.h
 * @author night1700 <night1700@night1700-15ZB970-GP50ML>
 * @date   Tue Apr 28 18:40:10 2020
 * 
 * @brief  
 * 
 * 
 */
#ifndef _REST_API_MESSAGE_H_
#define _REST_API_MESSAGE_H_


#define MAX_REST_MTYPE_LEN   		32	
#define MAX_REST_AUTHORIZATION_LEN	256
#define MAX_REST_TID_LEN   			8
#define MAX_REST_RESULT_CODE_LEN	8
#define MAX_REST_METHOD_LEN			12
#define MAX_REST_URI_LEN			512
#define MAX_PARAM_ID_LEN			64
#define MAX_REST_LOCATION_LEN		512
#define MAX_REST_CONTINUE_FLAG_LEN	4
#define MAX_REST_BODY_LEN_LEN		8

typedef struct {
	char 	mtype[MAX_REST_MTYPE_LEN];					// API별 MTYPE(추후 정의) enum -=> string  변홤
	char 	authorization[MAX_REST_AUTHORIZATION_LEN];	// Token, Auth Info // unuse
	char 	tid[MAX_REST_TID_LEN];						// Transaction ID    ///1증가
	char 	resCode[MAX_REST_RESULT_CODE_LEN];			// Response Code     // response 할때 사용.
	char 	method[MAX_REST_METHOD_LEN];				// API Method (GET, PUT, POST, DELETE) // post
	char 	uri[MAX_REST_URI_LEN];						// Full URI(ex. http://10.251.217.200:9091/../{sessionId} 
	char	param1Id[MAX_PARAM_ID_LEN];					// akey   // akey
    char	param2Id[MAX_PARAM_ID_LEN];					// remote ip addr   //상대 IP ~  보내는 ip
	char 	location[MAX_REST_LOCATION_LEN];			// location url     // unuse
	char 	continueFlag[MAX_REST_CONTINUE_FLAG_LEN];	// 0: COMPLETED, 1: CONTINUE  //unuse
	char 	bodyLen[MAX_REST_BODY_LEN_LEN];				// Json Body Length
} RestLibHeadType;
                         
#define MAX_JSON_BODY_LEN 		10000
typedef struct {
	RestLibHeadType	header;
	char	jsonBody[MAX_JSON_BODY_LEN];
} RestMsgType;

#endif
