static int msgpack_object_to_ra_value(msgpack_object o,
                                      struct flb_ra_value *result)
{
    result->o = o;

    /* Compose result with found value */
    if (o.type == MSGPACK_OBJECT_BOOLEAN) {
        result->type = FLB_RA_BOOL;
        result->val.boolean = o.via.boolean;
        return 0;
    }
    else if (o.type == MSGPACK_OBJECT_POSITIVE_INTEGER ||
             o.type == MSGPACK_OBJECT_NEGATIVE_INTEGER) {
        result->type = FLB_RA_INT;
        result->val.i64 = o.via.i64;
        return 0;
    }
    else if (o.type == MSGPACK_OBJECT_FLOAT32 ||
             o.type == MSGPACK_OBJECT_FLOAT) {
        result->type = FLB_RA_FLOAT;
        result->val.f64 = o.via.f64;
        return 0;
    }
    else if (o.type == MSGPACK_OBJECT_STR) {
        result->type = FLB_RA_STRING;
        result->val.string = flb_sds_create_len((char *) o.via.str.ptr,
                                                o.via.str.size);

        /* Handle cases where flb_sds_create_len fails */
        // <MASK>
    }
    else if (o.type == MSGPACK_OBJECT_MAP) {
        /* return boolean 'true', just denoting the existence of the key */
        result->type = FLB_RA_BOOL;
        result->val.boolean = true;
        return 0;
    }
    else if (o.type == MSGPACK_OBJECT_NIL) {
        result->type = FLB_RA_NULL;
        return 0;
    }

    return -1;
}