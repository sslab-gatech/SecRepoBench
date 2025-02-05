void* CMSEXPORT cmsReadTag(cmsHPROFILE hProfile, cmsTagSignature sig)
{
    _cmsICCPROFILE* Icc = (_cmsICCPROFILE*) hProfile;
    cmsIOHANDLER* io;
    cmsTagTypeHandler* TypeHandler;
    cmsTagTypeHandler LocalTypeHandler;
    cmsTagDescriptor*  TagDescriptor;
    cmsTagTypeSignature BaseType;
    cmsUInt32Number Offset, TagSize;
    cmsUInt32Number ElemCount;
    int n;

    if (!_cmsLockMutex(Icc->ContextID, Icc ->UsrMutex)) return NULL;

    n = _cmsSearchTag(Icc, sig, TRUE);
    // <MASK>
}