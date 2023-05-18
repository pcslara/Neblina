#ifndef __NEBLINASTD___
#define __NEBLINASTD___

#include "package.h"
extern cl_info clinfo;
extern     int nkernelsRmat;
#define new_str(i) (char *) malloc( sizeof(char)*(i))



package_t * getStdPackage();
 void ** neblina_type   ( void ** i, int * status );
 void ** vec_len        ( void ** i, int * status );
 void ** mat_len_col    ( void ** i, int * status );
 void ** mat_len_row    ( void ** i, int * status );
 void ** mat_mul_cpu    ( void ** i, int * status );
 void ** mat_mul        ( void ** i, int * status );
 void ** vec_add        ( void ** i, int * status );
 void ** vec_prod       ( void ** i, int * status );
 void ** vec_conj       ( void ** i, int * status );
 void ** vec_sub        ( void ** i, int * status );
 void ** vec_sub_cpu    ( void ** i, int * status );
 void ** vec_add_cpu    ( void ** i, int * status );
 void ** mat_add        ( void ** i, int * status );
 void ** vec_add_off    ( void ** i, int * status );
 void ** vec_add_off2   ( void ** i, int * status );
 void ** mat_add_cpu    ( void ** i, int * status );
 void ** mat_sub        ( void ** i, int * status );
 void ** mat_sub_cpu    ( void ** i, int * status );
 void ** vec_mulsc      ( void ** i, int * status );
 void ** vec_mulsc_cpu  ( void ** i, int * status );
 void ** mat_mulsc      ( void ** i, int * status );
 void ** mat_mulscrow   ( void ** i, int * status );
 void ** mat_mulsccol   ( void ** i, int * status );
 void ** mat_mulsc_cpu  ( void ** i, int * status );
 void ** matvec_mul1    ( void ** i, int * status );
 void ** matvec_mul2    ( void ** i, int * status );
 void ** matvec_mul3    ( void ** i, int * status );
 void ** matvec_mul_cpu ( void ** i, int * status );
 void ** smatvec_mul_cpu( void ** i, int * status );
 void ** mat_transp     ( void ** i, int * status );
 void ** mat_transp_cpu ( void ** i, int * status );
 void ** vec_sum        ( void ** i, int * status );
 void ** vec_sum_cpu    ( void ** i, int * status );

 void ** vec_norm       ( void ** i, int * status );
 void ** vec_norm_cpu   ( void ** i, int * status );
 void ** vec_dot        ( void ** i, int * status );
 void ** vec_dot_cpu    ( void ** i, int * status );
 void ** toint          ( void ** i, int * status );
 void ** tostr          ( void ** i, int * status );
 void ** tostr2         ( void ** i, int * status );
 void ** neblina_at     ( void ** i, int * status );
 void ** neblina_upper  ( void ** i, int * status );
 void ** neblina_lower  ( void ** i, int * status );
 void ** todouble       ( void ** i, int * status );
 void ** complex_new    ( void ** i, int * status );
 void ** complex_real   ( void ** i, int * status );
 void ** complex_imag   ( void ** i, int * status );
 void ** complex_conj   ( void ** i, int * status );
 void ** neblina_ludecomp( void ** i, int * status );
 void ** neblina_solve( void ** i, int * status );
 void ** neblina_list_new( void ** i, int * status );
 void ** neblina_list_append( void ** i, int * status );
 void ** neblina_list_get( void ** i, int * status );


 void ** neblina_mat_sqr( void ** i, int * s );

void ** init     ( void ** i, int * status );

 void ** neblina_rmatrix( void ** i, int * status );

#endif
