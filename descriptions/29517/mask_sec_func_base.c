static int
Mat_H5ReadGroupInfo(mat_t *mat,matvar_t *matvar,hid_t dset_id)
{
    int      fields_are_variables = 1;
    hsize_t  nfields=0,nelems;
    hid_t    attr_id,field_id;
    matvar_t **fields;
    H5O_type_t obj_type;
    int err;

    err = Mat_H5ReadVarInfo(matvar,dset_id);
    if ( err < 0 ) {
        return err;
    }

    /* Check if the variable is sparse */
    if ( H5Aexists_by_name(dset_id,".","MATLAB_sparse",H5P_DEFAULT) ) {
        herr_t herr;
        hid_t sparse_dset_id;
        unsigned nrows = 0;

        attr_id = H5Aopen_by_name(dset_id,".","MATLAB_sparse",H5P_DEFAULT,H5P_DEFAULT);
        herr = H5Aread(attr_id,H5T_NATIVE_UINT,&nrows);
        H5Aclose(attr_id);
        if ( herr < 0 ) {
            return MATIO_E_GENERIC_READ_ERROR;
        }

        matvar->class_type = MAT_C_SPARSE;

        sparse_dset_id = H5Dopen(dset_id,"jc",H5P_DEFAULT);
        matvar->dims = Mat_H5ReadDims(sparse_dset_id, &nelems, &matvar->rank);
        H5Dclose(sparse_dset_id);
        if ( NULL != matvar->dims ) {
            if ( 1 == matvar->rank ) {
                size_t* dims = (size_t*)realloc(matvar->dims, 2*sizeof(*matvar->dims));
                if ( NULL != dims ) {
                    matvar->rank = 2;
                    matvar->dims = dims;
                }
            }
            if ( 2 == matvar->rank ) {
                matvar->dims[1] = matvar->dims[0] - 1;
                matvar->dims[0] = nrows;
            }
        } else {
            return MATIO_E_UNKNOWN_ERROR;
        }

        /* Test if dataset type is compound and if so if it's complex */
        if ( H5Lexists(dset_id,"data",H5P_DEFAULT) ) {
            hid_t type_id;
            sparse_dset_id = H5Dopen(dset_id,"data",H5P_DEFAULT);
            type_id = H5Dget_type(sparse_dset_id);
            if ( H5T_COMPOUND == H5Tget_class(type_id) ) {
                /* FIXME: Any more checks? */
                matvar->isComplex = MAT_F_COMPLEX;
            }
            H5Tclose(type_id);
            H5Dclose(sparse_dset_id);
        }
        return MATIO_E_NO_ERROR;
    }

    // <MASK>
}