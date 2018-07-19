/*
*   @FILENAME: SerCliInterface.h
*
*/
#ifdef  __cplusplus
extern "C" {
#endif

/*!
SERVER和CLIENT间的MSG采样如下格式:

----------------------------------------------
16                                          1
----------------------------------------------
MSG_CONTENT LENGTH
----------------------------------------------
MSG_CONTENT
----------------------------------------------

*/

typedef struct
{
    unsigned short usCardId;
    unsigned short usDdrFlag;
    unsigned long  ulMsgType;
    unsigned long  ulSn;
}OSA_MSG_HEADER_ST;


typedef struct
{
    unsigned long       ulMsgLen;
    OSA_MSG_HEADER_ST   stMsgHeader;
    unsigned long       ulRsv;

}OSA_MSG_COMMON_ST;




#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
