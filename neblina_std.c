#include "neblina_std.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "package.h"
#include "libneblina.h"
#include "oclvector.h"
#include "clutils.h"
#include "neblina_list.h"
#include "neblina_vm.h"

extern cl_info clinfo;
extern     int nkernelsRmat;

void ** vec_len( void ** i, int * status ) {
    object_t out;
    object_t ** in = (object_t **) i;
    type( out ) = T_INT;
    if( type( *in[0] ) == T_VECTOR ) {
        vector_t * vec = (vector_t *)vvalue( *in[0] );
        ivalue( out ) = vec->len;  
    } else if( type( *in[0] ) == T_STRING ) {
        ivalue( out ) = strlen( svalue( *in[0] ) );
    } else if( type( *in[0] ) == T_LIST ) {
        ivalue( out ) = list_len(  (list_t *) vvalue( *in[0] ) );
    } else {
        runerror("invalid type input for function len()");      
    }
    static void * ret[1];
    clear_input(i, 1);
    ret[0] = (void *) &out;
    return ret;
}


 void ** mat_len_col( void ** i, int * status ) {
    object_t  out;// = (object_t *) malloc( sizeof( object_t ) );
    object_t ** in = (object_t **) i;
    type( out ) = T_INT;
    matrix_t * mat = (matrix_t *)vvalue( *in[0] );
    ivalue( out ) = mat->ncol;  
    static void * ret[1];
    clear_input( i, 1 );
    ret[0] = (void *) &out;
    return ret;
}


 void ** mat_len_row( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    if( type( *in[0] ) == T_MATRIX ) {
        object_t out;// = (object_t *) malloc( sizeof( object_t ) );

        type( out ) = T_INT;
        matrix_t * mat = (matrix_t *)vvalue( *in[0] );
        ivalue( out ) = mat->nrow;  
        static void * ret[1];
        clear_input( i, 1 );
        ret[0] = (void *) &out;
        return ret;
    } else {
        runerror( "Runtime error: no matrix found\n");
    }   
    return NULL;    
}

 void ** mat_mul_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        matrix_t * a = (matrix_t *) vvalue( *in[1] );
        matrix_t * b = (matrix_t *) vvalue( *in[0] );
        matreqhost( a ); matreqhost( b ); 
        object_t out;// = (object_t *) malloc( sizeof( object_t ) );
        matrix_t * r = matrix_multiply( a, b );
        r->location = LOCHOS;
        type( out ) = T_MATRIX;
        vvalue( out ) = (void *) r;
        static void * ret[1];
        clear_input( i, 2 );
        ret[0] = (void *) &out;
        return ret;
}

 void ** vec_add( void ** i, int * status ) {
        
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        vector_t * b = (vector_t *) vvalue( *in[1] );
        vecreqdev( a ); vecreqdev( b ); 
        int ii = 0;
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc( b->len * sizeof(double) );
       
        r->mem = addVectorF( a->mem, b->mem, b->len ); 
        
        
        r->len = b->len;
        r->type = T_FLOAT;
        r->location = LOCDEV;
        type( *out ) = T_VECTOR;
        vvalue( *out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** vec_conj( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        object_t out;
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        vecreqdev( a );
        r->value.f = (double *) malloc( a->len * sizeof(double) );
        r->mem = vecConj( a->mem, a->len ); 
    
        r->len = a->len;
        r->type = T_FLOAT;
        r->location = LOCDEV;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) &out;
        clear_input( i, 1 );
        return ret;
}
 void ** vec_conjugate( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        object_t out;
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        vecreqdev( a );

        r->value.f = (double *) malloc( 2*a->len * sizeof(double) );
        r->mem = vecConjugate( a->mem, a->len ); 
    
        r->len = a->len;
        r->type = T_COMPLEX;
        r->location = LOCDEV;
        type( out ) = T_VECTOR;
        
        vvalue( out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) &out;
        clear_input( i, 1 );
        return ret;
}
 void ** vec_prod( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        vector_t * b = (vector_t *) vvalue( *in[1] );
        vecreqdev( a ); vecreqdev( b );
        int ii = 0;
        object_t out;
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        if( a->type == T_FLOAT ) {
            r->value.f = (double *) malloc( b->len * sizeof(double) );
            r->mem = prodVector( a->mem, b->mem, b->len ); 
        } else {
            r->value.f = (double *) malloc( 2 * b->len * sizeof(double) );
            r->mem = prodComplexVector( a->mem, b->mem, b->len ); 
        }

        r->len = b->len;
        r->type = a->type;
        r->location = LOCDEV;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) &out;
        clear_input( i, 2 );
        return ret;
}

 void ** vec_sum( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        object_t  out;
        vecreqdev( a );
        fvalue( out ) = sumVector( a->mem, a->len );
        type( out ) = T_FLOAT;
        clear_input( i, 1 );        
        static void * ret[1];
        ret[0] = (void *) &out;
        return ret;
}

 void ** vec_norm( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vecreqdev( a );
        fvalue( *out ) = normVector( a->mem, a->len ); 
        type( *out ) = T_FLOAT;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}
 void ** vec_dot( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * v1 = (vector_t *) vvalue( *in[0] );
        vector_t * v2 = (vector_t *) vvalue( *in[1] );
        vecreqdev( v1 ); vecreqdev( v2 );
        object_t * out;
        if( type( *v1 ) == T_FLOAT && type( *v2 ) == T_FLOAT ) {
            out = (object_t *) malloc( sizeof( object_t ) );
            fvalue( *out ) = dotVector(v1->mem, v2->mem, v1->len); 
            type( *out ) = T_FLOAT;
        } else {
            out = (object_t *) malloc( sizeof( object_t ) );
            double re, im;
            dotVectorComplex(&re, &im, v1->mem, v2->mem, v1->len); 
            complex_t * res = (complex_t *) malloc( sizeof(complex_t) );
            res->im = im;
            res->re = re;
            vvalue( *out ) = (void *) res; 
            type( *out ) = T_COMPLEX;
         }
        
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** vec_dot_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * v1 = (vector_t *) vvalue( *in[0] );
        vector_t * v2 = (vector_t *) vvalue( *in[1] );
        vecreqhost( v1 );vecreqhost( v2 ); 
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        double sum = 0.0;
        int k = 0;
        for(k = 0; k < v1->len; k++ )
            sum += v1->value.f[k]*v2->value.f[k];
        
        fvalue( *out ) = sum; 
        type( *out ) = T_FLOAT;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** vec_norm_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vecreqhost( a );
        double sum = 0.0;
        int k = 0;
        for(k=0;k<a->len;k++) {
            sum += a->value.f[k]*a->value.f[k];
        }
        fvalue( *out ) = sqrt( sum ); 
        type( *out ) = T_FLOAT;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}
 void ** vec_sum_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vecreqhost( a );
        double sum = 0.0;
        int k = 0;
        for(k=0;k<a->len;k++) {
            sum += a->value.f[k];
        }
        fvalue( *out ) = sum; 
        type( *out ) = T_FLOAT;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** vec_sub( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        vector_t * b = (vector_t *) vvalue( *in[1] );
        vecreqdev( a ); vecreqdev( b );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc( b->len * sizeof(double) ); 
        r->mem = subVector( a->mem, b->mem, b->len ); 
        r->location = LOCDEV;
        r->len = b->len;
        r->type = T_FLOAT;
        type( *out ) = T_VECTOR;
        vvalue( *out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** vec_add_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        vector_t * b = (vector_t *) vvalue( *in[1] );
        vecreqhost( a );vecreqhost( b );
        object_t out;
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        int k;
        if( a->len != b->len )
            runerror( "invalid size of vectors" );
       
        
        if( a->type == T_FLOAT && b->type == T_FLOAT ) {
            r->value.f = (double *) malloc(b->len * sizeof(double));
            for(k=0;k<b->len;k++)
                r->value.f[k] = a->value.f[k] + b->value.f[k];
            r->type = T_FLOAT;
        } else if( a->type == T_COMPLEX && b->type == T_COMPLEX ) {
            r->value.f = (double *) malloc(2 * b->len * sizeof(double));
            for(k=0;k<2*b->len;k++)
                r->value.f[k] = a->value.f[k] + b->value.f[k];
            r->type = T_COMPLEX;
        } else if( (a->type == T_FLOAT && b->type == T_COMPLEX) || (a->type == T_COMPLEX && b->type == T_FLOAT) ) {
            r->value.f = (double *) malloc(2*b->len * sizeof(double));
            if( b->type == T_COMPLEX ) {
                vector_t * tmp = b;
                b = a;
                a = tmp;
            }
            
            for(k=0;k<b->len;k++) {
                r->value.f[2*k] = a->value.f[2*k] + b->value.f[k];
                r->value.f[2*k+1] = a->value.f[2*k+1];
            }
            r->type = T_COMPLEX;
        }             
        
        
        
        r->location = LOCHOS;
        r->len = b->len;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) &out;
        return ret;
}

 void ** vec_sub_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        vector_t * b = (vector_t *) vvalue( *in[1] );
        vecreqhost( a );vecreqhost( b );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc(b->len * sizeof(double));
        int k;
        for(k=0;k<b->len;k++)
            r->value.f[k] = a->value.f[k] - b->value.f[k];           
        r->location = LOCHOS;
        r->type = T_FLOAT;
        r->len = b->len;
        type( *out ) = T_VECTOR;
        vvalue( *out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** mat_add( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[0] );
    matrix_t * b = (matrix_t *) vvalue( *in[1] );
    matreqdev( a );matreqdev( b );
    object_t out;// = (object_t *) malloc( sizeof( object_t ) );
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    if( a->type == T_FLOAT && b->type == T_FLOAT ) { 
        r->value.f = (double *) malloc(b->nrow * b->ncol * sizeof(double));
        r->ncol = b->ncol;
        r->nrow = b->nrow;
        r->type = T_FLOAT;
        r->mem = addVectorF( a->mem, b->mem, b->nrow * b->ncol );
        r->location = LOCDEV;
        type( out ) = T_MATRIX;
        vvalue( out ) = (void *) r;
    }else if ( a->type == T_COMPLEX && b->type == T_COMPLEX) { 
        r->value.f = (double *) malloc(b->nrow * b->ncol  * 2 * sizeof(double));
        r->ncol = b->ncol;
        r->nrow = b->nrow;
        r->type = T_COMPLEX;
        r->mem = addVectorF( a->mem, b->mem, 2 * b->nrow * b->ncol );
        r->location = LOCDEV;
        type( out ) = T_MATRIX;
        vvalue( out ) = (void *) r;
    } else if((a->type == T_FLOAT && b->type == T_COMPLEX) ||
              (a->type == T_COMPLEX && b->type == T_FLOAT)) { 
     
        r->value.f = (double *) malloc(b->nrow * b->ncol  * 2 * sizeof(double));
        r->ncol = b->ncol;
        r->nrow = b->nrow;
        r->type = T_COMPLEX;
        if( a->type == T_FLOAT )
            r->mem = addVectorFC( a->mem, b->mem, b->nrow * b->ncol );
        else
            r->mem = addVectorFC( b->mem, a->mem, b->nrow * b->ncol );
        r->location = LOCDEV;
        type( out ) = T_MATRIX;
        vvalue( out ) = (void *) r;
    }
    
    static void * ret[1];
    ret[0] = (void *) &out;
    clear_input(i, 2);
    return ret;
}
 void ** mat_sub( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[1] );
    matrix_t * b = (matrix_t *) vvalue( *in[0] );
    matreqdev( a );matreqdev( b );
    object_t out;// = (object_t *) malloc( sizeof( object_t ) );
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    r->value.f = (double *) malloc(b->nrow * b->ncol * sizeof(double));
    r->ncol = a->ncol;
    r->nrow = a->nrow;
    r->type = T_FLOAT;
    r->mem = subVector( a->mem, b->mem, b->nrow * b->ncol );
    r->location = LOCDEV;
    type( out ) = T_MATRIX;
    vvalue( out ) = (void *) r;
    static void * ret[1];
    ret[0] = (void *) &out;
    clear_input(i, 2);
    return ret;
}
 void ** mat_mul( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[1] );
    matrix_t * b = (matrix_t *) vvalue( *in[0] );
    matreqdev( a );matreqdev( b );
    object_t out;// = (object_t *) malloc( sizeof( object_t ) );
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    if( a->type == T_FLOAT && b->type == T_FLOAT ) {
        r->type = T_FLOAT; 
        r->value.f = (double *) malloc(a->nrow * b->ncol * sizeof(double));
    } else if( (a->type == T_COMPLEX && b->type == T_COMPLEX) || 
             (a->type == T_FLOAT && b->type == T_COMPLEX) ) {
        r->type = T_COMPLEX;
        r->value.f = (double *) malloc(2 * a->nrow * b->ncol * sizeof(double));
    } else {
        runerror( "Invalid types for mat_mul\n" );
    }
        
    r->ncol = b->ncol;
    r->nrow = a->nrow;
    
    r->mem = matMul( a->mem, b->mem, a->nrow, b->ncol, a->ncol, a->type, b->type );
    r->location = LOCDEV;
    type( out ) = T_MATRIX;
    vvalue( out ) = (void *) r;
    static void * ret[1];
    ret[0] = (void *) &out;
    clear_input(i, 2);
    return ret;
}

 void ** neblina_mat_sqr( void ** i, int * s ) {
    object_t ** in = (object_t **) i;
    smatrix_t * a = (smatrix_t *) vvalue( *in[0] );
    double * aCol = (double *) malloc( a->maxcols * 2 * a->nrow * sizeof(double ) );
    int * aIdxCol = (int *) malloc( a->maxcols * a->nrow * sizeof(int) );
    cl_int status;
    
    smatrix_line_to_col( aCol, aIdxCol, a->m, a->idx_col, a->maxcols, a->nrow );
    int N = a->ncol;
    
    
    int maxcols =  a->maxcols;
    ord_smat( aCol, aIdxCol, maxcols, N );
    ord_smat( a->m, a->idx_col, maxcols, N );
    /*
    int j, jj, max = maxcols;
    for(jj=0; jj < N; jj++ ){
        for(j=0; j < max && aIdxCol[jj*max+j] != -1; j++ ) {
            printf("---> (%d, %d) [%f %f]\n", aIdxCol[jj*max+j], jj, aCol[2*(jj*max+j)], aCol[2*(jj*max+j)+1] ); 
        }
    }*/
    int * ptr1 = (int *)malloc(N * sizeof(int)), * ptr2= (int *)malloc(N * sizeof(int)), w;
    for(w=0;w<N;w++){ ptr1[w] = -1; ptr2[w] = -1; }
                
                
    cl_mem  outLin = clCreateBuffer( clinfo.c,  CL_MEM_WRITE_ONLY, 2 * N * maxcols * sizeof(double), NULL, &status);
    CLERR  
    cl_mem  outCol = clCreateBuffer( clinfo.c,  CL_MEM_WRITE_ONLY, 2 * N * maxcols * sizeof(double), NULL, &status);
    CLERR
    cl_mem  idxOutLin = clCreateBuffer( clinfo.c,  CL_MEM_USE_HOST_PTR, N * maxcols * sizeof(int), ptr1, &status);
    CLERR
    cl_mem  idxOutCol = clCreateBuffer( clinfo.c,  CL_MEM_USE_HOST_PTR, N * maxcols * sizeof(int), ptr2, &status);
    CLERR 
    
    cl_mem  mCol = clCreateBuffer( clinfo.c,  CL_MEM_USE_HOST_PTR, 2 * N * maxcols * sizeof(double), aCol, &status);
    CLERR
    cl_mem  idxCol = clCreateBuffer( clinfo.c,  CL_MEM_USE_HOST_PTR, N * maxcols * sizeof(int), aIdxCol, &status);
    CLERR  

    smatreqdev( a );
    matSquare( &outLin, &idxOutLin, 
               &outCol, &idxOutCol, 
               a->mMem, a->idxColMem, 
               mCol, idxCol, 
               maxcols, N );
    smatrix_t * ret = (smatrix_t *) malloc( sizeof(smatrix_t) );
    ret->nrow = a->nrow;
    ret->ncol = a->ncol;
    ret->maxcols = a->maxcols;
    ret->type = T_COMPLEX;
    ret->location = LOCDEV;
    ret->mMem = outLin;
    ret->idxColMem = idxOutLin;
    ret->idx_col = (int *) malloc(N * maxcols * sizeof(int));
    ret->m = (double *) malloc(2 * N * maxcols * sizeof(double));
    //smatreqhost( ret );    
    int ii;       
    /*for(ii=0;ii< N * maxcols;ii++)
        printf("idx[%i]=%d [%f %f]\n", ii, ret->idx_col[ii],  ret->m[2*ii],ret->m[2*ii+1] );*/
    object_t * out = (object_t *) malloc( sizeof( object_t ) );
    type( *out ) = T_SMATRIX;
    vvalue( *out ) = (void *) ret;
    static void * rr[1];
    rr[0] = (void *) out;
    return rr;
}

 void ** mat_transp( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[0] );
    matreqdev( a );
    object_t * out = (object_t *) malloc( sizeof( object_t ) );
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    r->value.f = (double *) malloc(a->nrow * a->ncol * sizeof(double));
    r->ncol = a->nrow;
    r->nrow = a->ncol;
    r->type = T_FLOAT;
    r->mem = matTranspose( a->mem, a->ncol , a->nrow );
    r->location = LOCDEV;
    type( *out ) = T_MATRIX;
    vvalue( *out ) = (void *) r;
    static void * ret[1];
    ret[0] = (void *) out;
    return ret;
}

 void ** mat_transp_cpu( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[0] );
    matreqhost( a );
    object_t * out = (object_t *) malloc( sizeof( object_t ) );
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    r->value.f = (double *) malloc(a->nrow * a->ncol * sizeof(double));
    r->nrow = a->ncol;
    r->ncol = a->nrow;
    r->type = T_FLOAT;
    int ii = 0, jj = 0;
    for( ii = 0; ii < r->nrow; ii++ )
        for( jj = 0; jj < r->ncol; jj++ )
            r->value.f[ii*r->nrow+jj] = a->value.f[jj*a->nrow+ii];
        
    r->location = LOCHOS;
    type( *out ) = T_MATRIX;
    vvalue( *out ) = (void *) r;
    static void * ret[1];
    ret[0] = (void *) out;
    return ret;
}


 void ** mat_sub_cpu( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[0] );
    matrix_t * b = (matrix_t *) vvalue( *in[1] );
    matreqhost( a );matreqhost( b );
    object_t * out = (object_t *) malloc( sizeof( object_t ) );
    int len = b->ncol * b->nrow;    
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    r->value.f = (double *) malloc(len * sizeof(double));
    r->ncol = b->ncol;
    r->nrow = b->nrow;
    r->type = T_FLOAT;    
    int k;
    for(k=0;k<len;k++)
        r->value.f[k] = b->value.f[k]-a->value.f[k];            
    r->type = T_FLOAT;
    r->location = LOCHOS;
    type( *out ) = T_MATRIX;
    vvalue( *out ) = (void *) r;
    static void * ret[1];
    ret[0] = (void *) out;
    return ret;
}

 void ** mat_add_cpu( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    matrix_t * a = (matrix_t *) vvalue( *in[0] );
    matrix_t * b = (matrix_t *) vvalue( *in[1] );
    matreqhost( a );matreqhost( b );
    object_t * out = (object_t *) malloc( sizeof( object_t ) );
    int len = b->ncol * b->nrow;    
    matrix_t * r = (matrix_t *) malloc( sizeof(matrix_t) );
    r->value.f = (double *) malloc(len * sizeof(double));
    r->ncol = b->ncol;
    r->nrow = b->nrow;
    r->type = T_FLOAT;    
    int k;
    for(k=0;k<len;k++)
        r->value.f[k] = a->value.f[k] + b->value.f[k];            
    r->type = T_FLOAT;
    r->location = LOCHOS;
    type( *out ) = T_MATRIX;
    vvalue( *out ) = (void *) r;
    static void * ret[1];
    ret[0] = (void *) out;
    return ret;
}

 void ** vec_mulsc( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        int k;
        double scalar = 1.0;  
        if( type( *in[0] ) == T_FLOAT )
            scalar = fvalue( *in[0] );
        else if( type( *in[0] ) == T_INT )
            scalar = (double) ivalue( *in[0] );
        else {
            // RUNTIME ERROR
        }
        printf("scalar = %f\n", scalar );
        vector_t * v = (vector_t *) vvalue( *in[1] );
        vecreqdev( v );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc(v->len * sizeof(double) );
        r->mem = mulScalarVector( v->mem, scalar, v->len ); 
        r->len = v->len;
        r->type = T_FLOAT;
        r->location = LOCDEV;
        type( *out ) = T_VECTOR;
        vvalue( *out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) out;
        // printf("return\n");
        return ret;
}

 void ** vec_mulsc_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        int k;
        double scalar = 1.0;  
        if( type( *in[0] ) == T_FLOAT )
            scalar = fvalue( *in[0] );
        else if( type( *in[0] ) == T_INT )
            scalar = (double) ivalue( *in[0] );
        else {
            // RUNTIME ERROR
        }
        vector_t * v = (vector_t *) vvalue( *in[1] );
        vecreqhost( v );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc(v->len * sizeof(double) );
        for(k=0; k<v->len;k++) {
            r->value.f[k] = scalar * v->value.f[k];
        }
        r->location = LOCHOS;
        r->len = v->len;
        r->type = T_FLOAT;
        type( *out ) = T_VECTOR;
        vvalue( *out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}

 void ** mat_mulscrow   ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    double scalar = 1.0;        
    if( type( *in[1] ) == T_FLOAT )
        scalar = fvalue( *in[1] );
    else if( type( *in[1] ) == T_INT )
        scalar = (double) ivalue( *in[1] );
    else {
         fprintf(stderr, "invalid scalar on 'mat_mulscrow'\n");
        exit( 1 );
    }
    
    int line = ivalue( *in[0] );
    
   
    matrix_t * m = (matrix_t *) vvalue( *in[2] );
    if( line > m->nrow ) {
        fprintf(stderr, "invalid line on 'mat_mulscrow'\n");
        exit( 1 );
    }
    
    matreqdev( m );
    line--;
    mulScalarMatRow( m->mem, scalar, m->nrow, m->ncol, line );        
    
    void * itoclear[2];
    itoclear[0] = i[0];
    itoclear[1] = i[1];
    clear_input(itoclear, 2);
        
    return NULL;    
}

 void ** mat_mulsccol  ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    double scalar = 1.0;        
    if( type( *in[1] ) == T_FLOAT )
        scalar = fvalue( *in[1] );
    else if( type( *in[1] ) == T_INT )
        scalar = (double) ivalue( *in[1] );
    else {
         // RUNTIME ERROR*
    }
    
    int col = ivalue( *in[0] );
    
    
    matrix_t * m = (matrix_t *) vvalue( *in[2] );
    if( col > m->ncol ) {
        fprintf(stderr, "invalid column on 'mat_mulsccol'\n");
        exit( 1 );
    }
    matreqdev( m );
    col--;
    mulScalarMatCol( m->mem, scalar, m->nrow, m->ncol, col );        
    
    void * itoclear[2];
    itoclear[0] = i[0];
    itoclear[1] = i[1];
    clear_input(itoclear, 2);
        
    return NULL;    
}
 void ** mat_mulsc( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        int k;
        double scalar = 1.0;        
        if( type( *in[0] ) == T_FLOAT )
            scalar = fvalue( *in[0] );
        else if( type( *in[0] ) == T_INT )
            scalar = (double) ivalue( *in[0] );
        else {
            // RUNTIME ERROR
        }

        matrix_t * m = (matrix_t *) vvalue( *in[1] );
        matreqdev( m );
        object_t  out;
        matrix_t * r = (matrix_t *) malloc( sizeof( matrix_t ) );
        if( m->type == T_FLOAT ) {
            r->value.f = (double *) malloc(m->nrow * m->ncol * sizeof(double) );
            r->mem = mulScalarVector( m->mem, scalar, m->nrow * m->ncol ); 
            r->nrow = m->nrow;
            r->ncol = m->ncol;
            r->type = T_FLOAT;
            r->location = LOCDEV;
            type( out ) = T_MATRIX;
            vvalue( out ) = (void *) r;
        } else if( m->type == T_COMPLEX ) {
            r->value.f = (double *) malloc(2 * m->nrow * m->ncol * sizeof(double) );
            r->mem = mulScalarVector( m->mem, scalar, 2 * m->nrow * m->ncol ); 
            r->nrow = m->nrow;
            r->ncol = m->ncol;
            r->type = T_COMPLEX;
            r->location = LOCDEV;
            type( out ) = T_MATRIX;
            vvalue( out ) = (void *) r;
        } 
        static void * ret[1];
        ret[0] = (void *) &out;
        clear_input(i, 2);
        return ret;
}

 void ** mat_mulsc_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        int k;
        double scalar = 1.0;        
        if( type( *in[0] ) == T_FLOAT )
            scalar = fvalue( *in[0] );
        else if( type( *in[0] ) == T_INT )
            scalar = (double) ivalue( *in[0] );
        else {
            // RUNTIME ERROR
        }

        matrix_t * m = (matrix_t *) vvalue( *in[1] );
        matreqhost( m );
        object_t * out = (object_t *) malloc( sizeof( object_t ) );
        matrix_t * r = (matrix_t *) malloc( sizeof( matrix_t ) );
        r->value.f = (double *) malloc(m->nrow * m->ncol * sizeof(double) );
        // int k = 0;
        for(k = 0; k < (m->nrow * m->ncol); k++ ) {
            r->value.f[k] = scalar * m->value.f[k];
        }
        r->location = LOCHOS;
        r->nrow = m->nrow;
        r->ncol = m->ncol;
        r->type = T_FLOAT;
        type( *out ) = T_MATRIX;
        vvalue( *out ) = (void *) r;
        static void * ret[1];
        ret[0] = (void *) out;
        return ret;
}



 void ** matvec_mul3( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * v = (vector_t *) vvalue( *in[0] );
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );        
        vecreqdev( v );
        object_t out;
        if( type( *in[1] ) == T_MATRIX ) {        

            matrix_t * m = (matrix_t *) vvalue( *in[1] );
            matreqdev( m );
            
            if( m->type == T_FLOAT && v->type == T_FLOAT ) {
                r->value.f = (double *) malloc(m->nrow * sizeof(double) );            
                r->mem = matVecMul3( m->mem, v->mem, m->ncol, m->nrow );
                r->location = LOCDEV;
                r->len = m->nrow;
                r->type = T_FLOAT;
            } else if( m->type == T_COMPLEX && v->type == T_COMPLEX ) {
                r->value.f = (double *) malloc(2*m->nrow * sizeof(double) );            
                r->mem = matVecMul3Complex( m->mem, v->mem, m->ncol, m->nrow );
                r->location = LOCDEV;
                r->len = m->nrow;
                r->type = T_COMPLEX;
            }
            type( out ) = T_VECTOR;
            vvalue( out ) = (void *) r;
            static void * ret[1];
            clear_input(i, 2);
            ret[0] = (void *) &out;
            return ret;
        } else  if( type( *in[1] ) == T_SMATRIX ) {
                smatrix_t * m = (smatrix_t *) vvalue( *in[1] );                
                smatreqdev( m );
                if( m->type == T_FLOAT && v->type == T_FLOAT ) {
                        
                    r->value.f = (double *) malloc(m->nrow * sizeof(double) );            
                    r->mem = sparseVecMul( m->mMem, m->idxColMem, v->mem, m->nrow, m->maxcols );
                    r->location = LOCDEV;
                    r->len = m->nrow;
                    r->type = T_FLOAT;
                    
                } else if( m->type == T_COMPLEX && v->type == T_COMPLEX ) {
                    r->value.f = (double *) malloc(2*m->nrow * sizeof(double) );            
                    r->mem = sparseComplexVecMul( m->mMem, m->idxColMem, v->mem, m->nrow, m->maxcols );
                    r->location = LOCDEV;
                    r->len = m->nrow;
                    r->type = T_COMPLEX;
                }
                type( out ) = T_VECTOR;
                vvalue( out ) = (void *) r;
                static void * ret[1];
                clear_input(i, 2);
                ret[0] = (void *) &out;
                return ret;
        } else if(  type( *in[1] ) == T_RMATRIX ) {
                rmatrix_t * m = (rmatrix_t *) vvalue( *in[1] );
                r->value.f = (double *) malloc(2*m->nrow * sizeof(double) );            
                r->mem = rmatVecMul3Complex( m, v->mem, m->ncol, m->nrow );
                r->location = LOCDEV;
                r->len = m->nrow;
                r->type = T_COMPLEX;
                type( out ) = T_VECTOR;
                vvalue( out ) = (void *) r;
                static void * ret[1];
                clear_input(i, 2);
                ret[0] = (void *) &out;
                return ret;
        } else {
            return (void **)NULL;   
        }
             
}

 void ** matvec_mul_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * v = (vector_t *) vvalue( *in[0] );
        matrix_t * m = (matrix_t *) vvalue( *in[1] );
        vecreqhost( v ); matreqhost( m );
        object_t out;
        vector_t * r = matvec_multiply( m, v );
        r->location = LOCHOS;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        static void * ret[1];
        clear_input( i, 2 );
        ret[0] = (void *) &out;
        return ret;
}

 void ** smatvec_mul_cpu( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        vector_t * v = (vector_t *) vvalue( *in[0] );
        smatrix_t * m = (smatrix_t *) vvalue( *in[1] );
        vecreqhost( v ); smatreqhost( m );
        
        object_t out;
        vector_t * r = smatvec_multiply( m, v );
        r->location = LOCHOS;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        static void * ret[1];
        clear_input( i, 2 );
        ret[0] = (void *) &out;
        return ret;
}



void ** init ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    if( type( *in[1] ) == T_VECTOR && (type(*in[0]) == T_FLOAT || type(*in[0]) == T_INT) ) {
        vector_t * v = (vector_t *) vvalue( *in[1] );
        vecreqhost( v );
        object_t * s = in[0];
        int i = 0;
        double f = (type(*in[0]) == T_FLOAT) ?  s->value.f : s->value.i;
        #pragma omp parallel for
        for( i=0;i < v->len; i++ )
            v->value.f[i] = f; 
    } else  if( type( *in[1] ) == T_MATRIX && (type(*in[0]) == T_FLOAT || type(*in[0]) == T_INT) ) {
        matrix_t * v = (matrix_t *) vvalue( *in[1] );
        matreqhost( v );
        object_t * s = in[0];
        int i = 0;
        double f = (type(*in[0]) == T_FLOAT) ?  s->value.f : s->value.i;
        int size = v->nrow * v->ncol;
        #pragma omp parallel for
        for( i=0;i <size; i++ )
            v->value.f[i] = f;
    } else {
        fprintf(stderr, "invalid argument on init\n");
        exit( 1 );
    }
    return (void **) NULL;
}

 void ** toint ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    type( out ) = T_INT;
    if( type( *in[0] ) == T_INT )
        ivalue( out ) = ivalue( *in[0] );
    else if( type( *in[0] ) == T_FLOAT )
        ivalue( out ) = fvalue( *in[0] );
    else if( type( *in[0] ) == T_STRING ) {
        ivalue( out ) = atoi( svalue( *in[0] ));
    } else {
        fprintf(stderr, "invalid use of 'toint' function\n");
        exit( 1 );
    }
    
    static void * ret[1];
    clear_input( i, 1 );
    ret[0] = (void *) &out;
    return ret;
}

 void ** todouble ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t * out = (object_t *) malloc( sizeof( object_t ) );
    type( *out ) = T_FLOAT;
    if( type( *in[0] ) == T_INT )
        fvalue( *out ) = ivalue( *in[0] );
    else if( type( *in[0] ) == T_FLOAT )
        fvalue( *out ) = fvalue( *in[0] );
    else if( type( *in[0] ) == T_STRING )
        fvalue( *out ) = atof( svalue( *in[0] ));
    else {
        fprintf(stderr, "invalid use of 'todouble' function\n");
        exit( 1 );
    }
    
    static void * ret[1];
    ret[0] = (void *) out;
    return ret;
}

 void ** tostr ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    type( out ) = T_STRING;
    if( type( *in[0] ) == T_INT ) {
        svalue( out ) = new_str( 32 );
        sprintf( svalue( out ), "%d", ivalue( *in[0] ) );  
    } else if( type( *in[0] ) == T_FLOAT ) {
        svalue( out ) = new_str( 64 );
        sprintf( svalue( out ), "%f", fvalue( *in[0] ) );
    } else {
        // runerror("invalid use of 'int' function");
        fprintf(stderr, "invalid use of 'tostr' function\n");
        exit( 1 );
    }
    
    static void * ret[1];
    clear_input(i,1);
    ret[0] = (void *) &out;
    return ret;
}

 void ** tostr2 ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    type( out ) = T_STRING;
    if( type( *in[1] ) == T_INT ) {
        svalue( out ) = new_str( 32 );
        char buf[6];
        sprintf( buf, "%%0%dd", ivalue( *in[0] ) );
        sprintf( svalue( out ), buf, ivalue( *in[1] ) );  
    } else if( type( *in[1] ) == T_FLOAT ) {
        svalue( out ) = new_str( 64 );
        sprintf( svalue( out ), "%f", fvalue( *in[1] ) );
    } else {
        // runerror("invalid use of 'int' function");
        fprintf(stderr, "invalid use of 'tostr' function\n");
        exit( 1 );
    }
    
    static void * ret[1];
    clear_input(i,2);
    ret[0] = (void *) &out;
    return ret;
}

 void ** complex_new ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t  out;// = (object_t *) malloc( sizeof( object_t ) );
    
    double re = 0;
    double im = 0;
    
    if( type( *in[1] ) == T_INT )
        re = ivalue( *in[1] );
    else if( type( *in[1] ) == T_FLOAT )
        re = fvalue( *in[1] );
    else {
        // runerror("invalid use of 'int' function");
        fprintf(stderr, "invalid use of 'complex' function\n");
        exit( 1 );
    }
    
    if( type( *in[0] ) == T_INT )
        im = ivalue( *in[0] );
    else if( type( *in[0] ) == T_FLOAT )
        im = fvalue( *in[0] );
    else {
        // runerror("invalid use of 'int' function");
        fprintf(stderr, "invalid use of 'complex' function\n");
        exit( 1 );
    }
    
    complex_t * res = (complex_t *) malloc( sizeof(complex_t) );
    res->im = im;
    res->re = re;
    
    type( out ) = T_COMPLEX;
    vvalue( out ) = (void *) res;
    
    //printf("Complex NEW\n");
    clear_input( i, 2);
    static void * ret[1];
    ret[0] = (void *) &out;
    return ret;
}

 void ** complex_real( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t  out;
    
    if( type( *in[0] ) != T_COMPLEX ) {
        
       fprintf(stderr, "invalid use of 'real' function\n");
       exit( 1 );
    }
    
    complex_t * r = (complex_t *) vvalue( *in[0] ); 
    
    type( out ) = T_FLOAT;
    fvalue( out ) = r->re;
    
    clear_input(i,1);
        
    static void * ret[1];
    ret[0] = (void *) &out;
    return ret;
}

 void ** complex_imag( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t  out;// = (object_t *) malloc( sizeof( object_t ) );
    
    if( type( *in[0] ) != T_COMPLEX ) {
        
       fprintf(stderr, "invalid use of 'imag' function\n");
       exit( 1 );
    }
    
    complex_t * r = (complex_t *) vvalue( *in[0] ); 
    
    type( out ) = T_FLOAT;
    fvalue( out ) = r->im;
    
    clear_input(i,1);
    static void * ret[1];
    ret[0] = (void *) &out;
    return ret;
}

 void ** complex_conj( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t  out;// = (object_t *) malloc( sizeof( object_t ) );
    
    
    complex_t * r = (complex_t *) vvalue( *in[0] ); 
    
    complex_t * res = (complex_t *) malloc( sizeof(complex_t) );
    res->im = -r->im;
    res->re = r->re;
    
    type( out ) = T_COMPLEX;
    vvalue( out ) = (void *) res;
    
    
    clear_input(i,1);
    static void * ret[1];
    ret[0] = (void *) &out;
    return ret;
}

 void ** neblina_at ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    type( out ) = T_STRING;
    int idx = ivalue( *in[0] );
    int len = strlen( svalue( *in[1] ) );
    
    if( (idx - 1) < 0 || (idx - 1) >= len ) {
        svalue( out ) = new_str(1);
        svalue( out )[0] = 0;
    } else {
        svalue( out ) = new_str(2);
        svalue( out )[0] = svalue( *in[1] )[idx-1];
        svalue( out )[1] = 0;
    }
    
    static void * ret[1];
    clear_input(i,2);
    ret[0] = (void *) &out;
    return ret;
}

 void ** neblina_upper( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    int k = 0;
    type( out ) = T_STRING;
    int len = strlen( svalue( *in[0] ) );
    svalue( out ) = new_str( len + 1 );
    
    for( k = 0; k < len; k++ )
        svalue( out )[k] = toupper(  svalue( *in[0] )[k] );
    svalue( out )[len] = 0;    
    
    static void * ret[1];
    clear_input(i,1);
    ret[0] = (void *) &out;
    return ret;
}

 void ** neblina_lower( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    int k = 0;
    type( out ) = T_STRING;
    int len = strlen( svalue( *in[0] ) );
    svalue( out ) = new_str( len + 1 );
    
    for( k = 0; k < len; k++ )
        svalue( out )[k] = tolower(  svalue( *in[0] )[k] );
    svalue( out )[len] = 0;    
    
    static void * ret[1];
    clear_input(i,1);
    ret[0] = (void *) &out;
    return ret;
}

 void ** neblina_type( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    int k = 0;
    type( out ) = T_STRING;
    char tmp[256];
    neblina_strtype( type( *in[0] ), tmp );
    int len = strlen( tmp );
    
    svalue( out ) = new_str( len + 1 );
    strcpy(svalue( out ), tmp );
    svalue( out )[len] = 0;
    
    static void * ret[1];
    clear_input(i,1);
    ret[0] = (void *) &out;
    return ret;
}
/*
 void ** vec_add_off    ( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    int offset = ivalue( *in[0] );
    
    vector_t * a = (vector_t *) vvalue( *in[1] );    
    vecreqhost( a );
    vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
    int parts = a->len / offset;
    r->value.f = (double *) malloc( offset * sizeof(double) );
    r->len = offset;
    r->type = T_FLOAT;
    r->location = LOCHOS;
    int j, l;    
    type( out ) = T_VECTOR;
    vvalue( out ) = r;
    double s = 0;
    #pragma omp parallel for
    for(j=0; j < r->len; j++ ) {
        s = 0;
        for(l=0; l < parts; l++ ) { 
            s += a->value.f[j+l*offset];   
        }
        r->value.f[j] = s;
    }
    static void * ret[1];
    clear_input(i,2);
    ret[0] = (void *) &out;
    return ret;
}*/


 void ** vec_add_off    ( void ** i, int * status ) {
    
        object_t ** in = (object_t **) i;
        int offset = ivalue( *in[0] );
        //printf("Off = %d\n", offset ); 
        vector_t * a = (vector_t *) vvalue( *in[1] );
        int parts = a->len / offset;
        vecreqdev( a );
        object_t out; 
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc( offset * sizeof(double) );
       
        r->mem = vecAddOff( a->mem, offset, parts ); 
        
        r->len = offset;
        r->type = T_FLOAT;
        r->location = LOCDEV;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        clear_input(i,2);    
        static void * ret[1];
        ret[0] = (void *) &out;
        return ret;
}

 void ** vec_add_off2   ( void ** i, int * status ) {
    
        object_t ** in = (object_t **) i;
        vector_t * a = (vector_t *) vvalue( *in[0] );
        vecreqdev( a );
        object_t out; 
        vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
        r->value.f = (double *) malloc( (a->len/2) * sizeof(double) );
       
        r->mem = vecAddOff2( a->mem, a->len ); 
        
        r->len = a->len/2;
        r->type = T_FLOAT;
        r->location = LOCDEV;
        type( out ) = T_VECTOR;
        vvalue( out ) = (void *) r;
        clear_input(i,1);    
        static void * ret[1];
        ret[0] = (void *) &out;
        return ret;
}

 void ** neblina_ludecomp( void ** i, int * status ) {
    object_t ** in = (object_t **) i;
    object_t out;
    
    matrix_t * m = (matrix_t *) vvalue( *in[0] );
    matreqdev( m );
    luDecomp( m->mem, m->nrow );  
    vvalue( out ) = (void *) m;
    m->location = LOCDEV;
    m->type = T_FLOAT;
    out.type = T_MATRIX; 
    // clear_input(i,1);
    static void * ret[1];
    ret[0] = (void *) &out;
    return ret;
}
 void ** neblina_solve( void ** I, int * status ) {
    object_t ** in = (object_t **) I;
    object_t out;
    
    matrix_t * A = (matrix_t *) vvalue( *in[1] );
    vector_t * b = (vector_t *) vvalue( *in[0] );
    matrix_t * M = (matrix_t *) malloc( sizeof(matrix_t) );
    M->value.f = (double *) malloc(A->nrow * A->ncol * sizeof(double));
    int n = A->nrow, i, j;
    matreqhost( A );
    #pragma omp parallel for
    for( i = 0; i < A->nrow * A->ncol ; i++){ M->value.f[i] = A->value.f[i]; }
    
    // memcpy(M->value.f, A->value.f, sizeof(double) * A->nrow * A->ncol );
    M->nrow = A->nrow;
    M->ncol = A->ncol;
    M->location = LOCHOS; 
    matreqdev( M ); vecreqhost( b );
    luDecomp( M->mem, M->nrow );  
    matreqhost( M );
    
    double * x = (double *) malloc(A->nrow  * sizeof(double));
    double * y = (double *) malloc(A->nrow  * sizeof(double));
    // Forward solve Ly = b
    for (i = 0; i < n; i++) {
        y[i] = b->value.f[i];
        for (j = 0; j < i; j++) {
          y[i] -= M->value.f[i*M->nrow + j] * y[j];
        }
        //y[i] /= M->value.f[i*M->nrow + i];
    }
    // Backward solve Ux = y
    for (i = n - 1; i >= 0; i--) {
        x[i] = y[i];
        for (j = i + 1; j < n; j++) {
          x[i] -= M->value.f[i*M->nrow + j] * x[j];
        }
        x[i] /= M->value.f[i*M->nrow + i];
    }
    // FREE Y
    free( y );
    // FREE
    //clReleaseMemObject( M->mem );  
    if( M && M->value.f ) {
        free( M->value.f );
        free( M );
    }
    
    
    
    vector_t * r = (vector_t *) malloc( sizeof( vector_t ) );
    r->value.f = x;
    r->len = A->nrow;   
    vvalue( out ) = (void *) r;
    r->location = LOCHOS;
    r->type = T_FLOAT;
    out.type = T_VECTOR; 
    clear_input(I,2);
    static void * ret[1];
    ret[0] = (void *) &out;
    return ret;
}



 void ** neblina_list_new( void ** i, int * status ) {
   object_t out;
   type( out ) = T_LIST;
   out.value.v = (void *) 0;
   static void * ret[1];
   ret[0] = (void *) &out;
   clear_input( i, 0 );   
   return ret;
}
 void ** neblina_list_append( void ** i, int * status ) {
        object_t ** in = (object_t **) i;
        list_t * lst = (list_t *) vvalue( *in[1] );
        object_t to_app = *in[0]; 
        
        lst = list_append( lst, to_app );
        object_t out;
   
       type( out ) = T_LIST;
       vvalue( out ) = lst;
       static void * ret[1];
       ret[0] = (void *) &out;
       clear_input( i, 2 );
       return ret;
}
 void ** neblina_list_get( void ** i, int * status ) {
       object_t ** in = (object_t **) i;
       list_t * lst = (list_t *) vvalue( *in[1] );
       object_t input = *in[0];
       int idx = ivalue(  input );
       
       object_t out = list_get( lst, idx );
       
       static void * ret[1];
       ret[0] = (void *) &out;
       clear_input( i, 2 );
       return ret;
 
}

 void ** neblina_rmatrix( void ** i, int * status ) {
       object_t ** in = (object_t **) i;
       int ncol = ivalue( *in[0] );
       int nrow = ivalue( *in[1] );
       char * getij = svalue(  *in[2] ); 
       
       char nkernel[4096];
       char kernelname[1024];
       int id = 0, ii;
       for(ii=0;ii<32;ii++)
            if( rmatstat[ii] != 0 ){
                id = ii;
                break;
            }
       sprintf( kernelname, RMATKERNAME, id );
       sprintf( nkernel, RMATKER, id, getij );
    //   printf("%s\n", nkernel );       
       cl_kernel kernel;
       rmatrix_t * rm = (rmatrix_t *) malloc( sizeof( rmatrix_t) );
       rm->ncol = ncol;
       rm->nrow = nrow;
       rm->id = id;
       strcpy( strkernelsRmat[id], nkernel );
       nkernelsRmat++;   
       rmatstat[id] = 1; 
       InitCLEngine();
       cl_int st;
       rm->kernel = clCreateKernel (clinfo.p, kernelname, &st);
       
       object_t out;
       type( out ) = T_RMATRIX;
       vvalue( out ) = (void *) rm;
       static void * ret[1];
       clear_input(i, 3);
       ret[0] = (void *) &out;
       return ret;
       
}



package_t * getStdPackage() {
    package_t * pkg   = package_init( 58, "std" );
    
    int ret_init[] = {};
    int input_init[] = { T_ANY, T_ANY };
    
    int ty_params[]   =  { T_VECTOR };
    int ty_returns []  = { T_INT };         
    int ty_params_m []   = { T_MATRIX };
    int ty_params_mul []   = { T_MATRIX, T_MATRIX };
    int ty_params_vecadd []  = {T_VECTOR, T_VECTOR};
    int ty_params_vecmulsc []  = {T_VECTOR, T_FLOAT};
    int ty_params_matvec []   = { T_ANY };
    int ty_params_2any []   = { T_ANY, T_ANY };
    int ty_params_mmv []   = { T_ANY, T_VECTOR };
    int ty_params_rmat []   = { T_STRING, T_INT, T_INT };
    
    int ty_params_matvecmul []   = { T_MATRIX, T_VECTOR };
    int ty_params_matmulsc []  = {T_MATRIX, T_FLOAT};
    
    int ty_params_matmulscrow []  = {T_MATRIX, T_ANY, T_INT};
    
    int ty_any [] = {T_ANY};
    int ty_int [] = {T_INT};
    int ty_double [] = {T_FLOAT};
    int ty_str [] = {T_STRING};
    int ty_complex [] = { T_COMPLEX };  
    int ty_at [] = { T_STRING, T_INT }; 
    
    int ty_list[] = { T_LIST };
    int ty_rmat[] = { T_RMATRIX };
    int ty_listapp[] = { T_LIST, T_ANY }; 
    int ty_listget[] = { T_LIST, T_INT }; 
    
    
    
    int n = 0;    
    
    pkg->functions[n++] = package_new_func( "len", 1, 1, ty_any, ty_returns, vec_len ); 
    pkg->functions[n++] = package_new_func( "nrows", 1, 1, ty_params_m, ty_returns, mat_len_row ); 
    pkg->functions[n++] = package_new_func( "ncols", 1, 1, ty_params_m, ty_returns, mat_len_col ); 
    pkg->functions[n++] = package_new_func( "mul", 2, 1, ty_params_mul, ty_params_m, mat_mul );
    pkg->functions[n++] = package_new_func( "vec_prod", 2, 1,ty_params_vecadd,ty_params, vec_prod );
    pkg->functions[n++] = package_new_func( "vec_add", 2, 1,ty_params_vecadd,ty_params, vec_add );
    pkg->functions[n++] = package_new_func( "vec_add_cpu", 2, 1,ty_params_vecadd,ty_params, vec_add_cpu );
    pkg->functions[n++] = package_new_func( "vec_sub", 2, 1,ty_params_vecadd,ty_params, vec_sub );
    pkg->functions[n++] = package_new_func( "vec_sub_cpu", 2, 1,ty_params_vecadd,ty_params, vec_sub_cpu );
    pkg->functions[n++] = package_new_func( "mat_add", 2, 1, ty_params_mul, ty_params_m, mat_add );
    pkg->functions[n++] = package_new_func( "mat_add_cpu", 2, 1, ty_params_mul, ty_params_m, mat_add_cpu );
    pkg->functions[n++] = package_new_func( "mat_sub", 2, 1, ty_params_mul, ty_params_m, mat_sub );
    pkg->functions[n++] = package_new_func( "mat_sub_cpu", 2, 1, ty_params_mul, ty_params_m, mat_sub_cpu );
    pkg->functions[n++] = package_new_func( "mat_mul", 2, 1, ty_params_mul, ty_params_m, mat_mul );
    pkg->functions[n++] = package_new_func( "mat_mul_cpu", 2, 1, ty_params_mul, ty_params_m, mat_mul_cpu );
    pkg->functions[n++] = package_new_func( "mat_transp", 1, 1, ty_params_mul, ty_params_m, mat_transp );
    pkg->functions[n++] = package_new_func( "mat_transp_cpu", 1, 1, ty_params_mul, ty_params_m, mat_transp_cpu );
    pkg->functions[n++] = package_new_func( "vec_mulsc", 2, 1,ty_params_vecmulsc, ty_params, vec_mulsc );
    pkg->functions[n++] = package_new_func( "vec_mulsc_cpu", 2, 1,ty_params_vecmulsc, ty_params, vec_mulsc_cpu );
    pkg->functions[n++] = package_new_func( "mat_mulscrow", 3, 0, ty_params_matmulscrow, ty_returns, mat_mulscrow );
    pkg->functions[n++] = package_new_func( "mat_mulsccol", 3, 0, ty_params_matmulscrow, ty_returns, mat_mulsccol );
    pkg->functions[n++] = package_new_func( "mat_mulsc", 2, 1,ty_params_matmulsc, ty_params_m, mat_mulsc );
    pkg->functions[n++] = package_new_func( "mat_mulsc_cpu", 2, 1,ty_params_matmulsc, ty_params_m, mat_mulsc_cpu );
    pkg->functions[n++] = package_new_func( "vec_sum", 1, 1,ty_params, ty_double, vec_sum );
    pkg->functions[n++] = package_new_func( "vec_add_off", 2, 1,input_init, ty_params, vec_add_off );
    pkg->functions[n++] = package_new_func( "vec_add_off", 1, 1,ty_params, ty_params, vec_add_off2 );
    pkg->functions[n++] = package_new_func( "vec_sum_cpu", 1, 1,ty_params, ty_double, vec_sum_cpu );
    pkg->functions[n++] = package_new_func( "vec_norm", 1, 1,ty_params, ty_double, vec_norm);
    pkg->functions[n++] = package_new_func( "vec_conj", 1, 1,ty_params, ty_params, vec_conj);
    pkg->functions[n++] = package_new_func( "vec_norm_cpu", 1, 1,ty_params, ty_double, vec_norm_cpu );
    pkg->functions[n++] = package_new_func( "vec_dot", 2, 1, ty_params_vecadd, ty_double, vec_dot);
    pkg->functions[n++] = package_new_func( "vec_dot_cpu", 2, 1, ty_params_vecadd, ty_double, vec_dot_cpu );
    pkg->functions[n++] = package_new_func( "mat_mulvec", 2, 1,ty_params_mmv , ty_params, matvec_mul3 );
    pkg->functions[n++] = package_new_func( "mat_mulvec_cpu", 2, 1,ty_params_mmv , ty_params, matvec_mul_cpu );
    pkg->functions[n++] = package_new_func( "smat_mulvec_cpu", 2, 1,ty_params_mmv , ty_params, smatvec_mul_cpu );
    pkg->functions[n++] = package_new_func( "movetodev", 1, 0,ty_params_matvec, ty_params, movetodev );
    pkg->functions[n++] = package_new_func( "movetohost", 1, 0,ty_params_matvec, ty_params, movetohost );
    pkg->functions[n++] = package_new_func( "init", 2, 0, input_init, ty_params , init );
    pkg->functions[n++] = package_new_func( "toint", 1, 1, ty_any, ty_int , toint  );
    pkg->functions[n++] = package_new_func( "todouble", 1, 1, ty_any, ty_double , todouble );
    pkg->functions[n++] = package_new_func( "tostr", 1, 1, ty_any, ty_str , tostr );
    pkg->functions[n++] = package_new_func( "tostr", 2, 1, input_init, ty_str , tostr2 );
    pkg->functions[n++] = package_new_func( "type", 1, 1, ty_any, ty_str , neblina_type );
    pkg->functions[n++] = package_new_func( "at", 2, 1, ty_at, ty_str , neblina_at );
    pkg->functions[n++] = package_new_func( "upper", 1, 1, ty_str, ty_str , neblina_upper );
    pkg->functions[n++] = package_new_func( "lower", 1, 1, ty_str, ty_str , neblina_lower );
    pkg->functions[n++] = package_new_func( "c", 2, 1, ty_params_2any, ty_complex , complex_new );
    pkg->functions[n++] = package_new_func( "real", 1, 1, ty_complex, ty_double , complex_real );
    pkg->functions[n++] = package_new_func( "imag", 1, 1, ty_complex, ty_double, complex_imag );
    pkg->functions[n++] = package_new_func( "conj", 1, 1, ty_complex, ty_complex, complex_conj );
    pkg->functions[n++] = package_new_func( "ludecomp", 1, 0, ty_params_m,ty_params_m, neblina_ludecomp );
    pkg->functions[n++] = package_new_func( "list", 0, 1, ty_list, ty_list, neblina_list_new );
    pkg->functions[n++] = package_new_func( "append", 2, 1, ty_listapp,ty_list, neblina_list_append );
    pkg->functions[n++] = package_new_func( "get", 2, 1, ty_listget,ty_list, neblina_list_get );
    pkg->functions[n++] = package_new_func( "mat_sqr", 1, 1, ty_any,ty_any, neblina_mat_sqr );
    pkg->functions[n++] = package_new_func( "rmatrix", 3, 1, ty_params_rmat,ty_rmat, neblina_rmatrix );
    pkg->functions[n++] = package_new_func( "vec_conjugate", 1, 1,ty_params, ty_params, vec_conjugate );
    pkg->functions[n++] = package_new_func( "solve", 2, 1,ty_params_mmv , ty_params, neblina_solve );
    
    
    return pkg;
}

