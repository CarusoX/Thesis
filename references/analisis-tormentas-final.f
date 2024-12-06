      implicit none

       integer i,j,k,s,tprom,mm,ttot,rate,umbral,cont,contt,ttprom
    
      parameter(tprom=1800)      !tiempo de promediado en segundos
      parameter(ttprom=120)   !tiempo de promedio en segundos para promedio temporal gotas positivas  y neativas
      parameter(mm=50000) !numero maximo de pulsos
      parameter(rate=5000)
      
      integer pasosprom
      parameter(pasosprom=1000) !numero maximo de pasos de promediado

      real q1(mm),q2(mm),d(mm),v(mm),drandom(mm)
      real o1b(mm), o1(mm),o2(mm),o3(mm),o4(mm),o5(mm),o(mm)
      integer ancho1(mm),ancho2(mm),t(mm),tlocal(mm)
      integer orden(mm),arch(mm),pulso(mm),bueno(mm)

      real q(mm),q1prom,q2prom
      real a1prom,a2prom
      
      integer m,num,pul,nu,pulbuenos
     
      integer can(pasosprom),cand
      real qp(pasosprom),eqp(pasosprom)
      real dp(pasosprom),edp(pasosprom)
      real qp_pos(pasosprom),eqp_pos(pasosprom), dp_pos(pasosprom)
      real edp_pos(pasosprom),edp_neg(pasosprom)
      real qp_neg(pasosprom),eqp_neg(pasosprom), dp_neg(pasosprom)
      real random,maxo
      real dprom
      real qd(0:24),dq(-100:100)
      integer cqd(0:24),cdq(-100:100)
      


************************************************
*******          ARCHIVOS               ********
************************************************

      open(unit=10, file='todos_los_datos.dat')

      open(unit=20, file='Histograma_car.dat')

      open(unit=30, file='Histograma_dia.dat')

      
      open(unit=40, file='car_vs_time_tod.dat')
  
      
      open(unit=50, file='dia_vs_time_tod.dat')
  
      
      open(unit=60, file='car_vs_dia_tod.dat')
 
  
      open(unit=70, file='car_vs_time_prom30_tod.dat')
      
      open(unit=80, file='dia_vs_time_prom30_tod.dat')
      open(unit=90, file='car_vs_time_prom30_pos.dat')
      
      open(unit=100, file='dia_vs_time_prom30_pos.dat')     
      open(unit=110, file='car_vs_time_prom30_neg.dat')
      open(unit=120, file='dia_vs_time_prom30_neg.dat')

           
      open(unit=130, file='car_prom_vs_dia_prom0.2_tod.dat')
      open(unit=140, file='car_prom_vs_dia_prom0.2_pos.dat')
      open(unit=150, file='car_prom_vs_dia_prom0.2_neg.dat')
      
       open(unit=160, file='car_prom_vs_dia_prom0.5_tod.dat')
      open(unit=170, file='car_prom_vs_dia_prom0.5_pos.dat')
      open(unit=180, file='car_prom_vs_dia_prom0.5_neg.dat')

      open(unit=41, file='car_vs_time_pos.dat')
      open(unit=42, file='car_vs_time_neg.dat')
      open(unit=51, file='dia_vs_time_pos.dat')
      open(unit=52, file='dia_vs_time_neg.dat')

*********************************************************
**********LECTURA DE DATOS ******************************
*********************************************************


      
      read(10,*) s,ttot

      pul=0
      do j=1,s
         read(10,*) m
         pul=pul+m
         do i=1,m
            read(10,*) arch(i+pul-m),orden(i+pul-m),tlocal(i+pul-m),
     &       t(i+pul-m),q1(i+pul-m),q2(i+pul-m),q(i+pul-m),d(i+pul-m),
     &       v(i+pul-m),ancho1(i+pul-m),ancho2(i+pul-m), o1b(i+pul-m),
     &      o1(i+pul-m),o2(i+pul-m),o3(i+pul-m),o4(i+pul-m),o5(i+pul-m),
     &       o(i+pul-m)
              enddo
           enddo
           
 
           write(*,*) 'cantidad de pulsos:',pul


**************FILTRO***************           
      pulbuenos=0

       
         do i=1,pul
c            if ((o1(i).le.5).and.(o2(i).le.5).and.
c     &         (o3(i).le.5).and.(o4(i).le.5).and.(o5(i).le.1)
c     &           .and.(o1(i)+o2(i)+o3(i)+o4(i)+o5(i).le.10)
c     &           .and.(abs(q(i)).lt.80).and.(abs(q(i)).gt.0.5)) then
c               
c               pulbuenos=pulbuenos+1
               bueno(i)=1
c     else
c               bueno(i)=0
c            endif
            enddo
            pulbuenos=pul
            write(*,*) 'pulsos buenos:',pulbuenos
            write(*,*) 'ojo no hay filtro, por eso da todos buenos'



**************HISTOGRAMA CARGA***************     
      contt=0
      write(20,*) -100.5,0,0     
      do j=-100,100
         cont=0        
         do i=1,pul
             if (bueno(i).eq.1) then
                if((q(i).le.real(j+1)).and.(q(i).gt.real(j))) then
                   cont=cont+1
                   contt=contt+1
                endif
              endif
         enddo
         write(20,*) j+0.5,cont,contt
      enddo

****************************
**************HISTOGRAMA DIAMETRO ***************     
      contt=0
       write(30,*) 0,0,0     
      do j=0,100,5
         cont=0        
         do i=1,pul
             if (bueno(i).eq.1) then
                if((d(i).le.real(j+5)/10.).and.
     &            (d(i).gt.real(j)/10.)) then
                   cont=cont+1
                   contt=contt+1
                endif
              endif
         enddo
         write(30,*) (j/10.)+0.25,cont,contt
      enddo

********************DIAMETRO PROMEDIO******************************
*********************************************************
          dprom=0.        
          cand=0
          do i=1,pul
             if (bueno(i).eq.1) then               
                dprom=dprom+d(i)                
                cand=cand+1
             endif
          enddo
            if (cand.ne.0) then
               dprom=dprom/real(cand)
            endif
          write(*,*) 'diametro promedio:',dprom 
      

**********GRAFICOS CARGA y DIAMETRO Vs TIEMPO******
**********DIAMETRO Vs CARGA******
**********CARGA Vs DIAMETRO******

         do i=1,pul
          if (bueno(i).eq.1) then
               write(40,*) t(i)/real(rate),q(i)
               write(50,*) t(i)/real(rate),d(i)
               write(60,*) d(i),q(i)
               if (q(i).ge.0) then
                  write(41,*) t(i)/real(rate),q(i)
                  write(51,*) t(i)/real(rate),d(i)
               else
                  write(42,*) t(i)/real(rate),q(i)
                  write(52,*) t(i)/real(rate),d(i)
               endif   
           endif
        enddo




*********************************************************
**********CALCULO DE CARGA Y DIAMETRO PROMEDIO TEMPORAL *********
******TODAS 
** CALCULO DE ERRORES DE CARGA Y DIAMETRO PROMEDIO ******      
*********************************************************

          num=ttot/(tprom*rate)

       do k=1,num+1
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if (bueno(i).eq.1) then               
                      if ((t(i).ge.(k-1)*tprom*rate).and.
     &                   (t(i).lt.k*tprom*rate)) then        
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                
                         can(k)=can(k)+1
                      endif
              endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo

          do k=1,num+1
            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
             if (bueno(i).eq.1) then
                  if ((t(i).ge.(k-1)*tprom*rate).and.
     &                (t(i).lt.k*tprom*rate)) then
                     eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                     edp(k)=edp(k)+(dp(k)-d(i))**2.               
                  endif
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            else
               eqp(k)=1
               edp(k)=1
            endif
          write(70,*) (real(k)-0.5)*tprom,qp(k),eqp(k)
          write(80,*) (real(k)-0.5)*tprom,dp(k),edp(k)
       enddo


*********************************************************
**********CALCULO DE CARGA Y DIAMETRO PROMEDIO TEMPORAL *********
******SOLO PARA GOTAS POSITIVAS
** CALCULO DE ERRORES DE CARGA Y DIAMETRO PROMEDIO ******      
*********************************************************
       
       do k=1,num+1
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if ((bueno(i).eq.1).and.(q(i).gt.0.)) then               
                      if ((t(i).ge.(k-1)*tprom*rate).and.
     &                   (t(i).lt.k*tprom*rate)) then        
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                
                         can(k)=can(k)+1
                      endif
              endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo

          do k=1,num+1
            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
             if ((bueno(i).eq.1).and.(q(i).gt.0.)) then
                  if ((t(i).ge.(k-1)*tprom*rate).and.
     &                (t(i).lt.k*tprom*rate)) then
                     eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                     edp(k)=edp(k)+(dp(k)-d(i))**2.               
                  endif
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            else
               eqp(k)=1
               edp(k)=1
            endif
          write(90,*) (real(k)-0.5)*tprom,qp(k),eqp(k)
          write(100,*) (real(k)-0.5)*tprom,dp(k),edp(k)
         enddo

       
*********************************************************
**********CALCULO DE CARGA Y DIAMETRO PROMEDIO TEMPORAL *********
******SOLO PARA GOTAS NEGATIVAS
** CALCULO DE ERRORES DE CARGA Y DIAMETRO PROMEDIO ******      
*********************************************************
     

       do k=1,num+1
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if ((bueno(i).eq.1).and.(q(i).lt.0.)) then               
                      if ((t(i).ge.(k-1)*tprom*rate).and.
     &                   (t(i).lt.k*tprom*rate)) then        
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                
                         can(k)=can(k)+1
                      endif
              endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo

          do k=1,num+1
            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
             if ((bueno(i).eq.1).and.(q(i).lt.0.)) then
                  if ((t(i).ge.(k-1)*tprom*rate).and.
     &                (t(i).lt.k*tprom*rate)) then
                     eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                     edp(k)=edp(k)+(dp(k)-d(i))**2.               
                  endif
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            elseif (can(k).eq.1) then
               eqp(k)=1
               edp(k)=1
            endif
          write(110,*) (real(k)-0.5)*tprom,qp(k),eqp(k)
          write(120,*) (real(k)-0.5)*tprom,dp(k),edp(k)
         enddo
    
*********************************************************
********CALCULO DE CARGA PROMEDIO vs DIAMETRO PROMEDIO
*********TODAS
********** CALCULO DE ERRORES CORRESPONDIENTES
**********************************************************     

       num=100

c$$$       do k=1,num
       do k=2,num,2
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
         
          do i=1,pul
             if (bueno(i).eq.1) then               
                 if ((d(i).ge.real(k-2)/10.).and.
     &                (d(i).lt.real(k)/10.)) then
c$$$                     if ((d(i).ge.real(k-1)/2.).and.
c$$$     &               (d(i).lt.real(k)/2.)) then
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                         
                         can(k)=can(k)+1
                     endif
              endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo
       
c$$$            do k=1,num
            do k=2,num,2

            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
             if (bueno(i).eq.1) then
                      if ((d(i).ge.real(k-1)/10.).and.
     &                     (d(i).lt.real(k)/10.)) then
c$$$                   if ((d(i).ge.real(k-1)/2.).and.
c$$$     &                (d(i).lt.real(k)/2.)) then
                     eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                     edp(k)=edp(k)+(dp(k)-d(i))**2.               
                  endif
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            else
             eqp(k)=1
             edp(k)=1  
          endif
          if (can(k).ne.0) then 
              write(130,*) dp(k),qp(k),edp(k),eqp(k)
          endif
       enddo

*********************************************************
********CALCULO DE CARGA PROMEDIO vs DIAMETRO PROMEDIO
********POSITIVAS
********** CALCULO DE ERRORES CORRESPONDIENTES
**********************************************************     


c$$$         do k=1,num
        do k=2,num,2   
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if (bueno(i).eq.1) then
                if (q(i).gt.0) then
                    if ((d(i).ge.real(k-2)/10.).and.
     &                  (d(i).lt.real(k)/10.)) then
c$$$                        if ((d(i).ge.real(k-1)/2.).and.
c$$$     &                       (d(i).lt.real(k)/2.)) then


                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                         
                         can(k)=can(k)+1
                         endif
                 endif
             endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo
       
c$$$          do k=1,num+1
          do k=2,num+1,2
             eqp(k)=0.
            edp(k)=0.
            do i=1,pul
               if (bueno(i).eq.1) then
                   if (q(i).gt.0) then
                      if ((d(i).ge.real(k-2)/10.).and.
     &                      (d(i).lt.real(k)/10.)) then
c$$$                        if ((d(i).ge.real(k-1)/2.).and.
c$$$     &                       (d(i).lt.real(k)/2.)) then
                      
                         eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                         edp(k)=edp(k)+(dp(k)-d(i))**2.               
                       endif
                   endif
                endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            else
             eqp(k)=1
             edp(k)=1  
           endif
           if (can(k).ne.0) then
              write(140,*) dp(k),qp(k),edp(k),eqp(k)
          endif
       enddo

*********************************************************
********CALCULO DE CARGA PROMEDIO vs DIAMETRO PROMEDIO
********NEGATIVAS
********** CALCULO DE ERRORES CORRESPONDIENTES
**********************************************************     


c$$$       do k=1,num
       do k=2,num+1,2   
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if (bueno(i).eq.1) then
                if (q(i).lt.0) then
                   if ((d(i).ge.real(k-2)/10.).and.
     &                  (d(i).lt.real(k)/10.)) then
c$$$                     if ((d(i).ge.real(k-1)/2.).and.
c$$$     &                    (d(i).lt.real(k)/2.)) then

   
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                         
                         can(k)=can(k)+1
                  endif
                endif
             endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo
       
c$$$       do k=1,num
       do k=2,num,2    
            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
               if (bueno(i).eq.1) then
                   if (q(i).lt.0) then
                      if ((d(i).ge.real(k-2)/10.).and.
     &                   (d(i).lt.real(k)/10.)) then
c$$$                         if ((d(i).ge.real(k-1)/2.).and.
c$$$     &                      (d(i).lt.real(k)/2.)) then
                        
                            eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                            edp(k)=edp(k)+(dp(k)-d(i))**2.               
                   endif
               endif
               
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
               eqp(k)=sqrt(eqp(k)/real(can(k)-1))
               edp(k)=sqrt(edp(k)/real(can(k)-1))
             else
                 eqp(k)=1
                 edp(k)=1 
           endif
          if (can(k).ne.0) then
             write(150,*) dp(k),qp(k),edp(k),eqp(k)
         endif
       enddo

*********************************************************
********CALCULO DE CARGA PROMEDIO vs DIAMETRO PROMEDIO
*********TODAS
********** CALCULO DE ERRORES CORRESPONDIENTES
**********************************************************     

       num=100

       do k=1,num
c$$$       do k=2,num,2
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
         
          do i=1,pul
             if (bueno(i).eq.1) then               
c$$$                 if ((d(i).ge.real(k-2)/10.).and.
c$$$     &                (d(i).lt.real(k)/10.)) then
                     if ((d(i).ge.real(k-1)/2.).and.
     &               (d(i).lt.real(k)/2.)) then
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                         
                         can(k)=can(k)+1
                     endif
              endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo
       
            do k=1,num
c$$$            do k=2,num,2

            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
             if (bueno(i).eq.1) then
c$$$                      if ((d(i).ge.real(k-1)/10.).and.
c$$$     &                     (d(i).lt.real(k)/10.)) then
                   if ((d(i).ge.real(k-1)/2.).and.
     &                (d(i).lt.real(k)/2.)) then
                     eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                     edp(k)=edp(k)+(dp(k)-d(i))**2.               
                  endif
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            else
             eqp(k)=1
             edp(k)=1  
          endif
          if (can(k).ne.0) then 
              write(160,*) dp(k),qp(k),edp(k),eqp(k)
          endif
       enddo

*********************************************************
********CALCULO DE CARGA PROMEDIO vs DIAMETRO PROMEDIO
********POSITIVAS
********** CALCULO DE ERRORES CORRESPONDIENTES
**********************************************************     


         do k=1,num
c$$$        do k=2,num,2   
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if (bueno(i).eq.1) then
                if (q(i).gt.0) then
c$$$                    if ((d(i).ge.real(k-2)/10.).and.
c$$$     &                  (d(i).lt.real(k)/10.)) then
                        if ((d(i).ge.real(k-1)/2.).and.
     &                       (d(i).lt.real(k)/2.)) then


                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                         
                         can(k)=can(k)+1
                         endif
                 endif
             endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo
       
          do k=1,num+1
c$$$          do k=2,num+1,2
             eqp(k)=0.
            edp(k)=0.
            do i=1,pul
               if (bueno(i).eq.1) then
                   if (q(i).gt.0) then
c$$$                      if ((d(i).ge.real(k-2)/10.).and.
c$$$     &                      (d(i).lt.real(k)/10.)) then
                        if ((d(i).ge.real(k-1)/2.).and.
     &                       (d(i).lt.real(k)/2.)) then
                      
                         eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                         edp(k)=edp(k)+(dp(k)-d(i))**2.               
                       endif
                   endif
                endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
             eqp(k)=sqrt(eqp(k)/real(can(k)-1))
             edp(k)=sqrt(edp(k)/real(can(k)-1))
            else
             eqp(k)=1
             edp(k)=1  
           endif
           if (can(k).ne.0) then
              write(170,*) dp(k),qp(k),edp(k),eqp(k)
          endif
       enddo

*********************************************************
********CALCULO DE CARGA PROMEDIO vs DIAMETRO PROMEDIO
********NEGATIVAS
********** CALCULO DE ERRORES CORRESPONDIENTES
**********************************************************     


        do k=1,num
c$$$       do k=2,num+1,2   
          qp(k)=0.
          dp(k)=0.        
          can(k)=0
          do i=1,pul
             if (bueno(i).eq.1) then
                if (q(i).lt.0) then
c$$$                   if ((d(i).ge.real(k-2)/10.).and.
c$$$     &                  (d(i).lt.real(k)/10.)) then
                     if ((d(i).ge.real(k-1)/2.).and.
     &                    (d(i).lt.real(k)/2.)) then

   
                         qp(k)=qp(k)+q(i)
                         dp(k)=dp(k)+d(i)                         
                         can(k)=can(k)+1
                  endif
                endif
             endif
          enddo
             if (can(k).ne.0) then
             qp(k)=qp(k)/real(can(k))
             dp(k)=dp(k)/real(can(k))             
            endif
       enddo
       
       do k=1,num
c$$$       do k=2,num,2    
            eqp(k)=0.
            edp(k)=0.
            do i=1,pul
               if (bueno(i).eq.1) then
                   if (q(i).lt.0) then
c$$$                      if ((d(i).ge.real(k-2)/10.).and.
c$$$     &                   (d(i).lt.real(k)/10.)) then
                         if ((d(i).ge.real(k-1)/2.).and.
     &                      (d(i).lt.real(k)/2.)) then
                        
                            eqp(k)=eqp(k)+(qp(k)-q(i))**2.
                            edp(k)=edp(k)+(dp(k)-d(i))**2.               
                   endif
               endif
               
               endif
            enddo
            if ((can(k).ne.0).and.(can(k).ne.1)) then
               eqp(k)=sqrt(eqp(k)/real(can(k)-1))
               edp(k)=sqrt(edp(k)/real(can(k)-1))
             else
                 eqp(k)=1
                 edp(k)=1 
           endif
          if (can(k).ne.0) then
             write(180,*) dp(k),qp(k),edp(k),eqp(k)
         endif
       enddo
       end       
   
