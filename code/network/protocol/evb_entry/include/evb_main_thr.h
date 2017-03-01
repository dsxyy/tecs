#if !defined(EVB_MAIN_THR_INCLUDE_H__)
#define EVB_MAIN_THR_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif    

    void EvbMainThrdEntry(void *pParam);
    void EvbTimerThrdEntry(void *pParam);

#ifdef __cplusplus
}
#endif

#endif
