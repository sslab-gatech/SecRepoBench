unsigned int boneIndex  = 0;
    SkipSpacesAndLineEnd(szCurrent,&szCurrent);
    if ( !ParseUnsignedInt(szCurrent,&szCurrent,boneIndex) || !SkipSpaces(szCurrent,&szCurrent)) {
        throw DeadlyImportError("Unexpected EOF/EOL while parsing bone index");
    }