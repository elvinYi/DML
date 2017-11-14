
#ifndef __H_IDataSource__
#define __H_IDataSource__


#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif

    // {E420D21E-7B3A-4dca-988B-E083AB0902F3}
    DECLARE_INTERFACE_(IDataSource, IUnknown)
    {

        STDMETHOD(SetData) (THIS_ unsigned char *pData) PURE;

    };

#ifdef __cplusplus
}
#endif

#endif
#endif
