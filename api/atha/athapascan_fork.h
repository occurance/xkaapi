/* KAAPI/Athapascan public interface */
// KAAPI library source
// -----------------------------------------
// by Thierry Gautier
//(c) INRIA, projet MOAIS, 2006-2010
//
// **********************************************************
// WARNING! This file has been automatically generated by M4
// vendredi 16 avril 2010, 16:16:39 (UTC+0200)
// *********************************************************















    /* 1 is the number of possible parameters */
    template<class E1, class F1>
    kaapi_task_t* PushArg( void (TASK::*)( F1 ), const E1& e1 )
    {
      typedef KaapiTaskArg1<TASK, F1> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      
      return clo;
    }

    template<class E1>
    void operator()( const E1& e1 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 2 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2 ), const E1& e1,const E2& e2 )
    {
      typedef KaapiTaskArg2<TASK, F1,F2> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      
      return clo;
    }

    template<class E1,class E2>
    void operator()( const E1& e1, const E2& e2 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 3 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3 ), const E1& e1,const E2& e2,const E3& e3 )
    {
      typedef KaapiTaskArg3<TASK, F1,F2,F3> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      
      return clo;
    }

    template<class E1,class E2,class E3>
    void operator()( const E1& e1, const E2& e2, const E3& e3 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 4 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4 )
    {
      typedef KaapiTaskArg4<TASK, F1,F2,F3,F4> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 5 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4, F5 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4,const E5& e5 )
    {
      typedef KaapiTaskArg5<TASK, F1,F2,F3,F4,F5> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E5>::mode, typename Trait_ParamClosure<F5>::mode, 
                ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      Trait_ParamClosure<F5>::link(arg->f5, e5);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4, const E5& e5 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4, e5 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 6 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4, F5, F6 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4,const E5& e5,const E6& e6 )
    {
      typedef KaapiTaskArg6<TASK, F1,F2,F3,F4,F5,F6> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E5>::mode, typename Trait_ParamClosure<F5>::mode, 
                ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E6>::mode, typename Trait_ParamClosure<F6>::mode, 
                ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      Trait_ParamClosure<F5>::link(arg->f5, e5);
      Trait_ParamClosure<F6>::link(arg->f6, e6);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4, const E5& e5, const E6& e6 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4, e5, e6 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 7 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4, F5, F6, F7 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4,const E5& e5,const E6& e6,const E7& e7 )
    {
      typedef KaapiTaskArg7<TASK, F1,F2,F3,F4,F5,F6,F7> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E5>::mode, typename Trait_ParamClosure<F5>::mode, 
                ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E6>::mode, typename Trait_ParamClosure<F6>::mode, 
                ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E7>::mode, typename Trait_ParamClosure<F7>::mode, 
                ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      Trait_ParamClosure<F5>::link(arg->f5, e5);
      Trait_ParamClosure<F6>::link(arg->f6, e6);
      Trait_ParamClosure<F7>::link(arg->f7, e7);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4, const E5& e5, const E6& e6, const E7& e7 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4, e5, e6, e7 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 8 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7, class E8, class F8>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4, F5, F6, F7, F8 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4,const E5& e5,const E6& e6,const E7& e7,const E8& e8 )
    {
      typedef KaapiTaskArg8<TASK, F1,F2,F3,F4,F5,F6,F7,F8> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E5>::mode, typename Trait_ParamClosure<F5>::mode, 
                ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E6>::mode, typename Trait_ParamClosure<F6>::mode, 
                ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E7>::mode, typename Trait_ParamClosure<F7>::mode, 
                ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E8>::mode, typename Trait_ParamClosure<F8>::mode, 
                ARG<8>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      Trait_ParamClosure<F5>::link(arg->f5, e5);
      Trait_ParamClosure<F6>::link(arg->f6, e6);
      Trait_ParamClosure<F7>::link(arg->f7, e7);
      Trait_ParamClosure<F8>::link(arg->f8, e8);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7,class E8>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4, const E5& e5, const E6& e6, const E7& e7, const E8& e8 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4, e5, e6, e7, e8 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 9 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7, class E8, class F8, class E9, class F9>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4, F5, F6, F7, F8, F9 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4,const E5& e5,const E6& e6,const E7& e7,const E8& e8,const E9& e9 )
    {
      typedef KaapiTaskArg9<TASK, F1,F2,F3,F4,F5,F6,F7,F8,F9> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E5>::mode, typename Trait_ParamClosure<F5>::mode, 
                ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E6>::mode, typename Trait_ParamClosure<F6>::mode, 
                ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E7>::mode, typename Trait_ParamClosure<F7>::mode, 
                ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E8>::mode, typename Trait_ParamClosure<F8>::mode, 
                ARG<8>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E9>::mode, typename Trait_ParamClosure<F9>::mode, 
                ARG<9>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      Trait_ParamClosure<F5>::link(arg->f5, e5);
      Trait_ParamClosure<F6>::link(arg->f6, e6);
      Trait_ParamClosure<F7>::link(arg->f7, e7);
      Trait_ParamClosure<F8>::link(arg->f8, e8);
      Trait_ParamClosure<F9>::link(arg->f9, e9);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7,class E8,class E9>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4, const E5& e5, const E6& e6, const E7& e7, const E8& e8, const E9& e9 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4, e5, e6, e7, e8, e9 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }















    /* 10 is the number of possible parameters */
    template<class E1, class F1, class E2, class F2, class E3, class F3, class E4, class F4, class E5, class F5, class E6, class F6, class E7, class F7, class E8, class F8, class E9, class F9, class E10, class F10>
    kaapi_task_t* PushArg( void (TASK::*)( F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 ), const E1& e1,const E2& e2,const E3& e3,const E4& e4,const E5& e5,const E6& e6,const E7& e7,const E8& e8,const E9& e9,const E10& e10 )
    {
      typedef KaapiTaskArg10<TASK, F1,F2,F3,F4,F5,F6,F7,F8,F9,F10> KaapiClosure;

      PassingRule<typename Trait_ParamClosure<E1>::mode, typename Trait_ParamClosure<F1>::mode, 
                ARG<1>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E2>::mode, typename Trait_ParamClosure<F2>::mode, 
                ARG<2>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E3>::mode, typename Trait_ParamClosure<F3>::mode, 
                ARG<3>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E4>::mode, typename Trait_ParamClosure<F4>::mode, 
                ARG<4>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E5>::mode, typename Trait_ParamClosure<F5>::mode, 
                ARG<5>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E6>::mode, typename Trait_ParamClosure<F6>::mode, 
                ARG<6>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E7>::mode, typename Trait_ParamClosure<F7>::mode, 
                ARG<7>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E8>::mode, typename Trait_ParamClosure<F8>::mode, 
                ARG<8>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E9>::mode, typename Trait_ParamClosure<F9>::mode, 
                ARG<9>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      PassingRule<typename Trait_ParamClosure<E10>::mode, typename Trait_ParamClosure<F10>::mode, 
                ARG<10>, FOR_TASKNAME<TASK> >::IS_COMPATIBLE();
      

      kaapi_task_t* clo = kaapi_thread_toptask( _thread );
      kaapi_task_initdfg( clo, KaapiClosure::default_body, kaapi_thread_pushdata(_thread, sizeof(KaapiClosure)) );
      /* this function call is the only way I currently found to register the format of the task, 
         idealy it should not be call and the clo->format should not be set at all.
      */
      KaapiClosure* arg = kaapi_task_getargst( clo, KaapiClosure);

      Trait_ParamClosure<F1>::link(arg->f1, e1);
      Trait_ParamClosure<F2>::link(arg->f2, e2);
      Trait_ParamClosure<F3>::link(arg->f3, e3);
      Trait_ParamClosure<F4>::link(arg->f4, e4);
      Trait_ParamClosure<F5>::link(arg->f5, e5);
      Trait_ParamClosure<F6>::link(arg->f6, e6);
      Trait_ParamClosure<F7>::link(arg->f7, e7);
      Trait_ParamClosure<F8>::link(arg->f8, e8);
      Trait_ParamClosure<F9>::link(arg->f9, e9);
      Trait_ParamClosure<F10>::link(arg->f10, e10);
      
      return clo;
    }

    template<class E1,class E2,class E3,class E4,class E5,class E6,class E7,class E8,class E9,class E10>
    void operator()( const E1& e1, const E2& e2, const E3& e3, const E4& e4, const E5& e5, const E6& e6, const E7& e7, const E8& e8, const E9& e9, const E10& e10 )
    {
      kaapi_task_t* clo = PushArg( &TASK::operator(), e1, e2, e3, e4, e5, e6, e7, e8, e9, e10 );
      _attr(_thread, clo );
      kaapi_thread_pushtask( _thread );    
    }
