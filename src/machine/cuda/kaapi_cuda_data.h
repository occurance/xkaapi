
#ifndef KAAPI_CUDA_DATA_H_INCLUDED
#define KAAPI_CUDA_DATA_H_INCLUDED

int kaapi_cuda_data_send( 
	kaapi_format_t*		   fmt,
	void*			sp
);

int kaapi_cuda_data_recv( 
	kaapi_format_t*		   fmt,
	void*	              sp
);

int kaapi_cuda_data_send_ptr( 
	kaapi_format_t*		   fmt,
	void*			sp
);

int kaapi_cuda_data_recv_ptr( 
	kaapi_format_t*		   fmt,
	void*	              sp
);

#endif