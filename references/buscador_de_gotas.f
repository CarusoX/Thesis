      implicit none

************************************************
**************** PARAMETROS ********************      
************************************************

*****************
      
      integer numarchmax,m,nn,pulsos
      
      parameter(numarchmax=25)
      ! numero maximo de tramos que esta dividida la medicion
          
      parameter(m=13000000)
      ! numero maximo de las variables t,v1,v2.
      
      parameter(nn=100)
      ! numero maximo de pasos que se adelanta buscando
      ! el pulso en la segunda senial a partir del 
      ! punto que encontro un pulso
      ! en la primera senial
      ! (debe ser numero par por comodidad de calculo)
      ! este numero es usado en muchas ocaciones
      ! vi que 100 es lo mejor
      ! para el caso de rate=5000

      parameter(pulsos=20000)
      ! maximo numero de pulsos que puede haber
      
******************
      
      real rate,separacion_anillo_olla,qminima
      real propcarga,propancho
      
      parameter(rate=5000.)
      ! variable que guarda la taza de adquisicion

      parameter(separacion_anillo_olla=0.057)
      ! 5.7 cm 

      parameter(qminima=0.2)
      ! minima carga electrica que se le permite
      ! que mida para la aplicacion de un filtro


      parameter(propcarga=0.87)      
      ! proporcion entre la carga del anillo y la olla

      parameter(propancho=0.79)      
      ! proporcion entre la ancho del anillo y la olla
      
******************
      
      real c1,c2,c3
      integer ps
      
      parameter(c1=2.00)
      ! umbral superior para buscar los pulsos
      
      parameter(c2=0.02)
      ! umbral inferior para buscar los pulsos
      
      parameter(c3=5.)
      ! exponencial determina la velocidad de caida
      ! del unmbral entre los umbrales superior e inferior
      ! hace que al principio caiga rapido y al
      ! final vaya mas lento.
      ! La forma funcional es:
      ! (1-1/k)**c3/(1-1/ps)**c3 *(c2-c1) +c1
      ! donde k varia desde 1 hasta ps
      
      parameter(ps=1000) 
      ! cantidad de 'pasos' que el umbral cae
      ! desde c1 a c2
      
******************      
      
************************************************
**************** VARIABLES  ********************      
************************************************

********** ENTERAS *********      
      integer numarch,t1(numarchmax),t2(numarchmax),pul(numarchmax)
      ! numarch = numero de tramos que esta dividida la medicion
      ! t1 = numero de pasos acumulada hasta antes de un
      ! determinado tramo
      ! t2 = numero de pasos de un determindo tramo
      ! pul= numero de pulsos encontrado en el tramo
      
      integer s,ss,i,j,w,k,l,ind(m)
      ! s =  cuenta el numero de tramos
      ! ss = sirve para poder calcular t1(pasos acumulados)
      ! i = cuenta los pasos en cada tramo
      ! j = cuenta los pasos (locales) en cada pulso
      ! j otro uso es contar pasos en forma auxiliar
      ! w = cuenta pasos en forma auxiliar
      ! l variable auxilar de pulsos
      ! k = cuenta los saltos que va cayendo el umbral
      ! ind = es el 'indice' que dice si hay que leer o no cada pulso

      integer u1(pulsos),u2(pulsos),p1(pulsos),p2(pulsos)
      ! u1 y u2 son los 'tiempos' (contado en pasos)
      ! donde arranca el pulso en la senial 1 y 2
      ! p1 y p2 son los 'tiempos' (contado en pasos)
      ! donde se alla el pico maximo del pulso en la senial 1 y 2
      
      integer p,r,x
      ! variables auxiliares para encontrar p2

      integer cont,contt
      ! contadores de pulsos buenos
      ! cont cuenta los pulsos por tramo
      ! contt cuenta el total de pulsos
      
 
      integer cc1,cc2,iii
      ! contadores auxiliares para calcular penal4
      
      integer indx(pulsos)
      ! indx = indice para ver si el pulso es bueno o no
      ! en el segundo filtro

      integer orden(pulsos)
      ! da el orden segun el coeficiente de penalizacion
      ! en cada tramo
      
**********REALES *********
      
      real t(m),v1(m),v2(m)
      ! variables de la seniales
      ! t es el tiempo (pero en realidad son canidad de pasos)
      ! cada paso es tiempo igual a 1/rate
      ! v1 y v2 son dos las seniales
      ! v1 es el ANILLO
      ! v2 es la OLLA

      real vc(1128),dc(1128)
      ! estas variable sirven para obtener el diametro
      ! de una gota sabiendo la velocidad de caida
      ! las velocidades varian entre 0.7125 m/s a 9.02385 m/s
      ! los diametros entre 0.175 mm a 5.81 mm
      
      real umbral
      ! es la variable 'umbral' que usa para buscar cada pulso
      
      real suma,ii(0:4*nn)
      ! variables auxiliares para encontrar p2

      real q1(pulsos),q2(pulsos),v(pulsos),d(pulsos)
      ! carga del cada pulso, velocidad y diametro

      real q(pulsos)
      ! carga promedio 'pesado' entre q1 y q2 

      real penal1(pulsos),penal1b(pulsos),penal2(pulsos),penal3(pulsos)
      real penal4(pulsos),penal5(pulsos),penal(pulsos)
      real prueba1(pulsos), prueba2(pulsos)
      ! variables que calculan los diferentes coeficientes
      ! de 'penalizacion'
      ! 'penal' es la suma de los cuatro
      
      real w1(0:4*nn,pulsos),w2(0:4*nn,pulsos),w1prom,w2prom
      ! variable que representa la senial de cada pulso
      ! la primera variable es el 'tiempo local de cada pulso'
      ! la segunda variable designa al numero de pulso
      
      real i1(0:4*nn,pulsos),i2(0:4*nn,pulsos)
      ! variable que resulta ser la integral de cada pulso
     
      real a1(0:4*nn,pulsos),a2(0:4*nn,pulsos), b1(0:4*nn,pulsos)
      ! variable que representa la idealizacion de la ser la integral     
      ! es el 'modelo' contra el que se compara las integrales
      ! verdaderas de cada pulso para calcular el pulso

      real x1,x2,y1,y2
      ! variables auxiliares para calcular penal3     

************************************************
**********  ARCHIVOS DE LECTURA INICIAL ********
************************************************

      open(unit=10, file='tiempos.dat')
      ! tiene la informacion de la cantidad de pasos
      ! de cada tramo
      
      open(unit=11, file='curva.dat')
      ! tiene la relacion velocidad-diametro de gotas
      ! para poder calcular el diametro con la velocidad
      ! terminal de caida

      
************************************************
***********     ARCHIVO ESCRITURA FINAL ********
************************************************
      
      open(unit=120, file='calculos/todos_los_datos.dat')

************************************************
****** PRIMERA LECTURA DE DATOS ****************
************************************************

c do i=1,1128
c    read(11,*) vc(i),dc(i)
c enddo
      ! lectura de la tabla velocidad terminal-diametro

      read(10,*) numarch
      ! lectura de la cantidad de tramos

      do s=1,numarch
         read(10,*) t2(s)       
         t1(s)=0 
      enddo
      ! lectura del numero de pasos de cada tramo
      ! puesta a cero del numero de pasos acumulados
      ! antes de cada tramo (debido a los tramos anteriores)

      do s=2,numarch
         do ss=s,2,-1
            t1(s)=t1(s)+t2(ss-1)
         enddo
      enddo
      ! calculo del numero de pasos anterior a cada tramo

****************************************************************
****** ESCRITURA EN PANTALLA DE LOS PASOS DE TRAMOS ************
****************************************************************

      
      write(120,*) numarch,t1(numarch)+t2(numarch) 
      
      write(*,*)

      write(*,*) '    archivo      duracion    inicio       final'

      write(*,*)
      do s=1,numarch
         write(*,*) s,t2(s),t1(s),t2(s)+t1(s)
      enddo
      write(*,*)
      write(*,*)

      write(*,*) 'archivo  pulsos(sin segundo filtro)   pulsos   total'
      write(*,*)

      contt=0 ! se pone a cero el numero total de pulsos buenos
      
************************************************
****** INICIO DEL LOOP  MAYOR   ****************
************************************************

      do s=1,numarch

      ! barre tramos de la medicion   

************************************************
****** LECTURA DE DATOS DE CADA TRAMO **********
************************************************

      if (s.eq.1) then   
       
      open(unit=20, file='20-02-04-01/20-02-04-01-prom.dat')
      open(unit=30, file='20-02-04-01/20-02-04-01-ver.dat')
     
      elseif (s.eq.2) then
         
      open(unit=20, file='20-02-04-02/20-02-04-02-prom.dat')
      open(unit=30, file='20-02-04-02/20-02-04-02-ver.dat')

      elseif (s.eq.3) then
         
      open(unit=20, file='20-02-04-03/20-02-04-03-prom.dat')
      open(unit=30, file='20-02-04-03/20-02-04-03-ver.dat')

      elseif (s.eq.4) then
         
      open(unit=20, file='20-02-04-04/20-02-04-04-prom.dat')
      open(unit=30, file='20-02-04-04/20-02-04-04-ver.dat')

      elseif (s.eq.5) then
         
      open(unit=20, file='20-02-04-05/20-02-04-05-prom.dat')
      open(unit=30, file='20-02-04-05/20-02-04-05-ver.dat')

      elseif (s.eq.6) then
         
      open(unit=20, file='20-02-04-06/20-02-04-06-prom.dat')
      open(unit=30, file='20-02-04-06/20-02-04-06-ver.dat')

      elseif (s.eq.7) then
         
      open(unit=20, file='20-02-04-07/20-02-04-07-prom.dat')
      open(unit=30, file='20-02-04-07/20-02-04-07-ver.dat')

      elseif (s.eq.8) then
         
      open(unit=20, file='20-02-04-08/20-02-04-08-prom.dat')
      open(unit=30, file='20-02-04-08/20-02-04-08-ver.dat')

      elseif (s.eq.9) then
         
      open(unit=20, file='20-02-04-09/20-02-04-09-prom.dat')
      open(unit=30, file='20-02-04-09/20-02-04-09-ver.dat')

      elseif (s.eq.10) then
         
      open(unit=20, file='20-02-04-10/20-02-04-10-prom.dat')
      open(unit=30, file='20-02-04-10/20-02-04-10-ver.dat')

      elseif (s.eq.11) then
            
      open(unit=20, file='20-02-04-11/20-02-04-11-prom.dat')
      open(unit=30, file='20-02-04-11/20-02-04-11-ver.dat')
      
      elseif (s.eq.12) then
         
      open(unit=20, file='20-02-04-12/20-02-04-12-prom.dat')
      open(unit=30, file='20-02-04-12/20-02-04-12-ver.dat')

      elseif (s.eq.13) then
         
      open(unit=20, file='20-02-04-13/20-02-04-13-prom.dat')
      open(unit=30, file='20-02-04-13/20-02-04-13-ver.dat')

      elseif (s.eq.14) then
         
      open(unit=20, file='20-02-04-14/20-02-04-14-prom.dat')
      open(unit=30, file='20-02-04-14/20-02-04-14-ver.dat')

      elseif (s.eq.15) then
         
      open(unit=20, file='20-02-04-15/20-02-04-15-prom.dat')
      open(unit=30, file='20-02-04-15/20-02-04-15-ver.dat')

      elseif (s.eq.16) then
         
      open(unit=20, file='20-02-04-16/20-02-04-16-prom.dat')
      open(unit=30, file='20-02-04-16/20-02-04-16-ver.dat')

      elseif (s.eq.17) then
         
      open(unit=20, file='20-02-04-17/20-02-04-17-prom.dat')
      open(unit=30, file='20-02-04-17/20-02-04-17-ver.dat')

      elseif (s.eq.18) then
         
      open(unit=20, file='20-02-04-18/20-02-04-18-prom.dat')
      open(unit=30, file='20-02-04-18/20-02-04-18-ver.dat')

      elseif (s.eq.19) then
         
      open(unit=20, file='20-02-04-19/20-02-04-19-prom.dat')
      open(unit=30, file='20-02-04-19/20-02-04-19-ver.dat')

      elseif (s.eq.20) then
         
      open(unit=20, file='20-02-04-20/20-02-04-20-prom.dat')
      open(unit=30, file='20-02-04-20/20-02-04-20-ver.dat')     
c$$$
c$$$      elseif (s.eq.21) then
c$$$            
c$$$      open(unit=20, file='20-02-04-21/20-02-04-21-prom.dat')
c$$$      open(unit=30, file='20-02-04-21/20-02-04-21-ver.dat')
c$$$      
c$$$      elseif (s.eq.22) then
c$$$         
c$$$      open(unit=20, file='20-02-04-22/20-02-04-22-prom.dat')
c$$$      open(unit=30, file='20-02-04-22/20-02-04-22-ver.dat')
c$$$
c$$$      elseif (s.eq.23) then
c$$$         
c$$$      open(unit=20, file='20-02-04-23/20-02-04-23-prom.dat')
c$$$      open(unit=30, file='20-02-04-23/20-02-04-23-ver.dat')
c$$$
c$$$      elseif (s.eq.24) then
c$$$         
c$$$      open(unit=20, file='20-02-04-24/20-02-04-24-prom.dat')
c$$$      open(unit=30, file='20-02-04-24/20-02-04-24-ver.dat')
c$$$
c$$$      elseif (s.eq.25) then
c$$$         
c$$$      open(unit=20, file='20-02-04-25/20-02-04-25-prom.dat')
c$$$      open(unit=30, file='20-02-04-25/20-02-04-25-ver.dat')
c$$$
c$$$      elseif (s.eq.26) then
c$$$         
c$$$      open(unit=20, file='20-02-04-26/20-02-04-26-prom.dat')
c$$$      open(unit=30, file='20-02-04-26/20-02-04-26-ver.dat')
c$$$
c$$$      elseif (s.eq.27) then
c$$$         
c$$$      open(unit=20, file='20-02-04-27/20-02-04-27-prom.dat')
c$$$      open(unit=30, file='20-02-04-27/20-02-04-27-ver.dat')
c$$$
c$$$      elseif (s.eq.28) then
c$$$         
c$$$      open(unit=20, file='20-02-04-28/20-02-04-28-prom.dat')
c$$$      open(unit=30, file='20-02-04-28/20-02-04-28-ver.dat')
c$$$
c$$$      elseif (s.eq.29) then
c$$$         
c$$$      open(unit=20, file='20-02-04-29/20-02-04-29-prom.dat')
c$$$      open(unit=30, file='20-02-04-29/20-02-04-29-ver.dat')
c$$$
c$$$      elseif (s.eq.30) then
c$$$         
c$$$      open(unit=20, file='20-02-04-30/20-02-04-30-prom.dat')
c$$$      open(unit=30, file='20-02-04-30/20-02-04-30-ver.dat')
c$$$
c$$$      elseif (s.eq.31) then
c$$$            
c$$$      open(unit=20, file='20-02-04-31/20-02-04-31-prom.dat')
c$$$      open(unit=30, file='20-02-04-31/20-02-04-31-ver.dat')
c$$$      
c$$$      elseif (s.eq.32) then
c$$$         
c$$$      open(unit=20, file='20-02-04-32/20-02-04-32-prom.dat')
c$$$      open(unit=30, file='20-02-04-32/20-02-04-32-ver.dat')
c$$$
c$$$      elseif (s.eq.33) then
c$$$         
c$$$      open(unit=20, file='20-02-04-33/20-02-04-33-prom.dat')
c$$$      open(unit=30, file='20-02-04-33/20-02-04-33-ver.dat')
c$$$
c$$$      elseif (s.eq.34) then
c$$$         
c$$$      open(unit=20, file='20-02-04-34/20-02-04-34-prom.dat')
c$$$      open(unit=30, file='20-02-04-34/20-02-04-34-ver.dat')
c$$$
c$$$      elseif (s.eq.35) then
c$$$         
c$$$      open(unit=20, file='20-02-04-35/20-02-04-35-prom.dat')
c$$$      open(unit=30, file='20-02-04-35/20-02-04-35-ver.dat')
c$$$
c$$$      elseif (s.eq.36) then
c$$$         
c$$$      open(unit=20, file='20-02-04-36/20-02-04-36-prom.dat')
c$$$      open(unit=30, file='20-02-04-36/20-02-04-36-ver.dat')
  
      endif

      do i=1,t2(s)
         read(20,*) t(i),v1(i),v2(i),ind(i)
      enddo
  
************************************************
****** CERADO DE VARIABLES *********************
************************************************      

      do i=1,pulsos
         u1(i)=0
         u2(i)=0
         p1(i)=0
         p2(i)=0
         indx(i)=1
         q1(i)=0.
         q2(i)=0.
         q(i)=0.
         penal1(i)=0.
         penal1b(i)=0.         
         penal2(i)=0.
         penal3(i)=0.
         penal4(i)=0.
         penal5(i)=0.         
         penal(i)=0.
         orden(i)=0
         d(i)=0.
         v(i)=0.
         do j=0,4*nn
            w1(j,i)=0.
            w2(j,i)=0.
            i1(j,i)=0.
            i2(j,i)=0.
            a1(j,i)=0.
            a2(j,i)=0.
            b1(j,i)=0.            
         enddo
      enddo
      
         do j=0,4*nn
            ii(j)=0.
         enddo
         
      umbral=0.
      suma=0.
      
************************************************
***** PRIMER FILTRO ****************************      
************************************************

      pul(s)=0 !va contando el numero de pulsos para cada porcion

      do k=1,ps

         umbral=(1.-1./real(k))**c3*(c2-c1)/
     &        (1.-1./real(ps))**c3+c1

      write(*,*) umbral
         
      ! esta funciom varia el umbral del para buscar el los pulsos
      ! hace que baje desde c1 hacia c2
      ! de una forma mas abrubta al principio (cerca de c1) y
      ! mas lenta al final (cerca de c2)
      ! La verdad no es muy importante la forma de esta funcion
      ! solo es importante que va rapido al principio y lento despues   
     
  
      do i=1,t2(s)   

************************************************
********  BUSQUEDA DE GOTAS              *******         
************************************************
         
*********************************************         
*********** Toda esta parte es para picos que
***********  arrancan hacia valores positivos
*********************************************
         
           if ((v1(i).ge.umbral).and.
     &         (v1(i+1).ge.umbral).and.
     &         (ind(i).eq.1)) then
       ! es necesario que dos puntos consecutivos superen el umbral
             
           do j=1,nn
       ! buscamos el segundo pulso hasta un total de nn puntos adelante
                 if ((v2(i+j).ge.umbral).and.
     &               (v2(i+j+1).ge.umbral).and.
     &               (ind(i+j).eq.1)) then

                    pul(s)=pul(s)+1
       ! si cumple lo anterior es un pulso y acumula en pul(s)
            
                    u1(pul(s))=i-nn
       ! define inicialmente la posicion que
       ! se empezara a contar el pulso de la
       ! primera senial como maximo nn puntos detras de i
                    
                    do w=1,nn-1
                         if (v1(i-w).le.0.) then
                            u1(pul(s))=i-w
                            goto 101
                         endif
                     enddo
       ! si ocurriera que la senial se hace
       ! cero antes que en la posicion maxima
       ! entonces redefine la posicion inicial
                     
 101                 u2(pul(s))=i+j-nn
        ! define inicialmente la posicion que
        ! se empezara a contar el pulso de la
        ! segunda senial como maximo nn puntos detras de i

                     do w=1,nn-1
                         if (v2(i+j-w).le.0.) then
                            u2(pul(s))=i+j-w
                            goto 102
                         endif
                      enddo
       ! si ocurriera que la senial se hace
       ! cero antes que en la posicion maxima
       ! entonces redefine la posicion inicial
                      
 102                  if (u2(pul(s)).lt.u1(pul(s))) then
                         u2(pul(s))=u1(pul(s))
                      endif
            ! no puede ocurrir que el inicio del segundo pulso
            ! sea anterior al primero, en tal caso es forzado a ser igual

***************************
***** busqueda del punto medio del primer pulso
***************************
                      
              do p=i,i+3*nn 
                  if ((v1(p).lt.0.).and.(v1(p+1).lt.0.)) then                
                     goto 103
                  endif   
               enddo
               
        ! buscamos el primer tiempo donde se anula
        ! dos puntos consecutivos despues de haber encontrado el pulso
              
 103           p1(pul(s))=p-1
               ! punto medio del primer pico que
               ! nos ayudara a calacular la carga del primer pulso


*************************
***** busqueda de punto de quiebre de la integral del segundo pulso
*************************
               
               ii(0)=0.
               do p=1,4*nn
                  ii(p)=ii(p-1)-v2(u2(pul(s))+p)
               enddo
         ! integracion (cambiada de signo)(del segundo pulso)
         ! desde la posicion u2(pul(s)+1) hasta u2(pul(s)+4*nn)
               
               do p=1,4*nn
                  if (ii(p).lt.-0.1) then 
                     goto 104
                  endif
               enddo
               ! el instante que la integral negativa
               ! supera -0.1 (en valor absoluto)
               ! es lo que se define como el primer punto
               ! en que veremos como baja la integral
               ! es el punto 'u2(pul(s))+p'
               
 104           do r=p,4*nn 
                  suma=0.
                  do x=p,r
                     suma=suma+v2(u2(pul(s))+x)/real(r-p+1) 
                  enddo 
 	          if (v2(u2(pul(s))+r+1).lt.0.3*suma) then                      
	              goto 105
	           endif
	        enddo
             ! variamos el extremo superior hasta donde
             ! calculamos los promedios de los
             ! puntos partiendo desde u2(pul)+p
             ! calculamos la pendiente de la 'recta integral'                
             ! si este punto es menor que 0.3 del promedio              
             ! anteriores entonces ocurre el quiebre de la integral,
             ! el 0.3 fue obtenido por observacion
                
 105            p2(pul(s))=u2(pul(s))+r
             ! punto de quiebre del segundo pulso

                

**************************************
*******   creacion de los recortes de los pulsos             
**************************************
                
                if (u2(pul(s))+r+nn/2.lt.u1(pul(s))+3*nn/2) then
            ! caso en que el segundo pulso es muy angosto
            ! se toma como minimo 3*nn/2 despues del inicio del primer pulso
                      do l=u1(pul(s)),u1(pul(s))+3*nn/2
                             ind(l)=0 ! anulacion del indice en ese parte 
                             w1(l-u1(pul(s)),pul(s))=v1(l)
                             w2(l-u1(pul(s)),pul(s))=v2(l)
                       enddo
                    else
                       if (u2(pul(s))+r+nn/2.le.u1(pul(s))+4*nn) then
            ! caso en que el segundo pulso es muy ancho,
            ! se toma como maximo r+nn/2 despues del inicio del segundo pulso
                         do l=u1(pul(s)),u2(pul(s))+r+nn/2
                             ind(l)=0
                             w1(l-u1(pul(s)),pul(s))=v1(l) 
                             w2(l-u1(pul(s)),pul(s))=v2(l)                      
                          enddo
                      else
                         do l=u1(pul(s)),u1(pul(s))+4*nn
            ! caso intermedio,se toma como maximo 4*nn
            ! despues del inicio primer pulso
                             ind(l)=0
                             w1(l-u1(pul(s)),pul(s))=v1(l)
                             w2(l-u1(pul(s)),pul(s))=v2(l)                      
                      enddo
                    endif     
                endif
                      
                 endif
               enddo
             endif
*********************************************         
*********** Toda esta parte es para picos que
***********  arrancan hacia valores negativos
*********************************************
             
           if ((v1(i).le.-umbral).and.
     &         (v1(i+1).le.-umbral).and.
     &         (ind(i).eq.1)) then
              
              do j=1,nn
                 if ((v2(i+j).le.-umbral).and.
     &               (v2(i+j+1).le.-umbral).and.
     &               (ind(i+j).eq.1)) then

                    pul(s)=pul(s)+1
            
                    u1(pul(s))=i-nn
                    do w=1,nn-1
                         if (v1(i-w).ge.0.) then
                            u1(pul(s))=i-w
                            goto 201
                         endif
                     enddo

 201                 u2(pul(s))=i+j-nn

                     do w=1,nn-1
                         if (v2(i+j-w).ge.0.) then
                            u2(pul(s))=i+j-w
                            goto 202
                         endif
                      enddo

 202                  if (u2(pul(s)).lt.u1(pul(s))) then
                         u2(pul(s))=u1(pul(s))
                      endif

              do p=i,i+3*nn
                  if ((v1(p).gt.0.).and.(v1(p+1).gt.0.)) then               
                     goto 203
                  endif   
              enddo

 203          p1(pul(s))=p-1
              
               ii(0)=0.
               do p=1,4*nn
                  ii(p)=ii(p-1)-v2(u2(pul(s))+p)
               enddo
               
               do p=1,4*nn
                  if (ii(p).gt.0.1) then
                     goto 204
                  endif
               enddo

 204           do r=p,4*nn
                  suma=0.
                  do x=p,r
                     suma=suma+v2(u2(pul(s))+x)/real(r-p+1)
                  enddo
                  
  	          if (v2(u2(pul(s))+r+1).gt.0.3*suma) then
	              goto 205
	           endif
	        enddo
     
                
 205            p2(pul(s))=u2(pul(s))+r
              
                
                if (u2(pul(s))+r+nn/2.lt.u1(pul(s))+3*nn/2) then
                      do l=u1(pul(s)),u1(pul(s))+3*nn/2
                             ind(l)=0
                             w1(l-u1(pul(s)),pul(s))=v1(l)
                             w2(l-u1(pul(s)),pul(s))=v2(l)                      
                       enddo
                    else
                       if (u2(pul(s))+r+nn/2.le.u1(pul(s))+4*nn) then
                         do l=u1(pul(s)),u2(pul(s))+r+nn/2
                             ind(l)=0
                             w1(l-u1(pul(s)),pul(s))=v1(l)
                             w2(l-u1(pul(s)),pul(s))=v2(l)                      
                          enddo
                      else
                      do l=u1(pul(s)),u1(pul(s))+4*nn
                             ind(l)=0
                             w1(l-u1(pul(s)),pul(s))=v1(l)
                             w2(l-u1(pul(s)),pul(s))=v2(l)                      
                      enddo
                    endif     
                 endif
                 
                 endif
               enddo
             endif
             
       enddo
      enddo


************************************************
****** Termino el bluque que todos lo pulsos
****** encontrados un correspondiente tramo de la medicion
****** es decir termino el primer filtro     
************************************************

      
************************************************************************
*****  RECORRIDO DE LOS PULSOS PARA CALCULAR CARGA Y VELOCIDAD *********
*****  APLICACION DEL SEGUNDO FILTRO                           *********      
************************************************************************
      
       do l=1,pul(s)    

************************************************
********** INTEGRACION DE PULSOS ENCONTRADOS ***
************************************************      

*********** INTEGRACION CANAL 1 ****************
          ! Recordemos:
          ! u1(l) es el inicio del pulso y
          ! p1(l) es el maximo del pulso,
          ! entonces 2*(p1(l)-u1(l)) es el ancho total
          if (2.*(p1(l)-u1(l)).lt.4*nn) then 
             do j=1,2*(p1(l)-u1(l))
                i1(j,l)=i1(j-1,l)-w1(j,l) 
             enddo
	  endif
          ! El signo menos es porque es inversor
          
           if (2*(p1(l)-u1(l)).ge.4*nn) then
             do j=1,4*nn
                i1(j,l)=i1(j-1,l)-w1(j,l)
             enddo
           endif
           ! como estamos graficando como maximo 4*nn,
           ! si el ancho total es mayor solo integramos hasta 4*nn

**********INTEGRACION CANAL 2 *****************
           ! Recordemos:
           ! u2(l) es el inicio del pulso y
           ! p2(l) es el punto de quiebre
           
           if ((p2(l)-u1(l)+nn/2).lt.4*nn) then 
              do j=u2(l)-u1(l)+1,p2(l)-u1(l)+nn/2 
                 i2(j,l)=i2(j-1,l)-w2(j,l)
              enddo
	   endif
           ! El signo menos es porque es inversor

           if ((p2(l)-u1(l)+nn/2).ge.4*nn) then
              do j=u2(l)-u1(l)+1,4*nn 
                 i2(j,l)=i2(j-1,l)-w2(j,l)
              enddo
	   endif
           
*********  pico maximo canal 1 y 2 *************
           
           q1(l)=2.*1000./rate*i1(p1(l)-u1(l),l) !carga del primer pulso
           
           q2(l)=2.*1000./rate*i2(p2(l)-u1(l),l) !carga del segundo pulso
           
!     Esta es la carga que ve el anillo y la olla en PicoCoulombs.
!     Explicacion del factor 2:
!     Como la amplificacion del amplificador de corriente es de 5x10^8 V/A
!     Cada V(voltios) de salida del amplificador (v1 y v2, w1 y w2)
!     corresponde a:
!     2x10^(-9) A (amperes) =(5x10^8)^(-1)
!     Como es integrado cada 1 milisegundo=10^(-3) s
!     (pues la grabacion fue a 1000 tomas por segundo)
!     Cada unidad de variables de integracion i1 y i2 representa:
!     2x10^(-9) A * 10^(-3) s=  2x10^(-12) C = 2 picocouloms  

!     Ese factor 2. es para cuando se integra 1000 tomas por segundo.
!     Cuando se integra por otra tasa se arregla multiplicando por 1000./rate
!     Por ejemplo si rate=3000. entonces el factor queda:
!     2.*1000./3000.=0.66666666
!     Por ejemplo si rate=5000. entonces el factor queda: 2.*1000./5000.=0.4    

           q(l)=(q1(l)/propcarga+q2(l))/2.

!     carga promedio 'pesado' entre anillo y olla
   
           
************************************************
***** SEGUNDO FILTRO ***************************      
************************************************

           if (abs(q2(l)).lt.qminima) then 
              indx(l)=0
           endif
           ! minimo de carga en la olla
           
           if (abs(q1(l)).lt.qminima) then

              indx(l)=0
           endif           
           ! minimo de carga en el anillo
           
           if (p1(l).ge.p2(l)-nn/4) then
              indx(l)=0
           endif
           ! minima diferencia entre picos
           ! (velocidad maxima de 11.4 m/s)
           
           if (p2(l).ge.p1(l)+4*nn) then
              indx(l)=0
           endif
           ! maxima diferencia entre picos
           ! (para que de velocidad minima de 0.7125 m/s)
		          
           if ((q1(l).ge.0.).and.(q2(l).le.0.)) then 
              indx(l)=0
           endif
           ! no pueden ser de diferente signo
           
           if ((q1(l).le.0.).and.(q2(l).ge.0.)) then 
              indx(l)=0
           endif
           ! no pueden ser de diferente signo


           if (indx(l).eq.1) then

         
            v(l)=separacion_anillo_olla/(t(p2(l))-t(p1(l)))*rate
              
********** MODELO DEL PRIMER PULSO *****************
          ! la varable a1 es un modelo inventado para compararlo
          ! con los pulsos reales

            
             if (2*(p1(l)-u1(l)).lt.4*nn) then
                 do j=0,2*(p1(l)-u1(l))
                     b1(j,l)=q1(l)*
     &                       exp(-((real(j-p1(l)+u1(l)))**2./
     &                       (real(p1(l)-u1(l)))**2.)*3.62)
                    
                     a1(j,l)=30.6*q1(l)/(30.6**(1/1.7)
     &                    +(v(l)/50.7)**2.*real(j-p1(l)+u1(l))**2.)**1.7
                 enddo
             endif
               
              if (2*(p1(l)-u1(l)).ge.4*nn) then
                do j=0,4*nn
                     b1(j,l)=q1(l)*
     &                       exp(-((real(j-p1(l)+u1(l)))**2./
     &                       (real(p1(l)-u1(l)))**2.)*3.62)

                     a1(j,l)=30.6*q1(l)/(30.6**(1/1.7)
     &                  +(v(l)/50.7)**2.*real(j-p1(l)+u1(l))**2.)**1.7
                     
                enddo
              endif

              
********** MODELO DEL SEGUNDO PULSO *****************
              ! la varable a2 es un modelo inventado
              ! para compararlo con los pulsos reales               
              ! es una rampa hasta el punto de quiebre que es constante
              
             if (p2(l)-u1(l).lt.4*nn) then
                 do j=u2(l)-u1(l)+1,p2(l)-u1(l)
       a2(j,l)=q2(l)*real(j-u2(l)+u1(l))**1.373/real(p2(l)-u2(l))**1.373
                 enddo
                 do j=p2(l)-u1(l)+1,4*nn
                    a2(j,l)=q2(l)
                 enddo
              endif
              if (p2(l)-u1(l).ge.4*nn) then
                 do j=u2(l)-u1(l)+1,4*nn
       a2(j,l)=q2(l)*real(j-u2(l)+u1(l))**1.373/real(p2(l)-u2(l))**1.373
                 enddo
              endif


**********CALCULO DE DIFERENCIAS DE CUADRADOS ********
!     Diferencia cuadratica entre las mediciones (i1,i2)
!     y los modelos (a1,a2)              
!     La variable penal1 es igual a la suma de todas las
!     diferencias al cuadrado
*****************************************************
***** ESTA ES PRIMERA PENALIZACION             
*****************************************************             

              if (2*(p1(l)-u1(l)).lt.4*nn) then
                  do j=0,2*(p1(l)-u1(l))
        penal1(l)=penal1(l)+((a1(j,l)-2.*1000./rate*i1(j,l))/q1(l))**2.
                  enddo
               endif
               if (2*(p1(l)-u1(l)).ge.4*nn) then
                  do j=0,4*nn
        penal1(l)=penal1(l)+((a1(j,l)-2.*1000./rate*i1(j,l))/q1(l))**2.
                  enddo
               endif

               penal1(l)=log(penal1(l)+1.)

               if (2*(p1(l)-u1(l)).lt.4*nn) then
                  do j=0,2*(p1(l)-u1(l))
        penal1b(l)=penal1b(l)+((b1(j,l)-2.*1000./rate*i1(j,l))/q1(l))**2.
                  enddo
               endif
               if (2*(p1(l)-u1(l)).ge.4*nn) then
                  do j=0,4*nn
        penal1b(l)=penal1b(l)+((b1(j,l)-2.*1000./rate*i1(j,l))/q1(l))**2.
                  enddo
               endif

               penal1b(l)=log(penal1b(l)+1.)              
              
****************
 
               if (p2(l)-u1(l).lt.4*nn-nn/2) then

                do j=0,p2(l)-u1(l)
        penal2(l)=penal2(l)+((a2(j,l)-2.*1000./rate*i2(j,l))/q2(l))**2.
                 enddo
                 do j=p2(l)-u1(l)+1,p2(l)-u1(l)+nn/2
        penal2(l)=penal2(l)+((a2(j,l)-2.*1000./rate*i2(j,l))/q2(l))**2.
                 enddo

              endif
              
               if (p2(l)-u1(l).ge.4*nn-nn/2) then

                 do j=0,p2(l)-u1(l)
        penal2(l)=penal2(l)+((a2(j,l)-2.*1000./rate*i2(j,l))/q2(l))**2.
                 enddo
                 do j=p2(l)-u1(l)+1,4*nn
        penal2(l)=penal2(l)+((a2(j,l)-2.*1000./rate*i2(j,l))/q2(l))**2.
                 enddo
              endif
              
              penal2(l)=log(penal2(l)+1.)              
             
**********CALCULO DE SEGUNDA PENALIZACION ********
!     La variable 'penal2' cuantifica cuanto el cociente
!     q1(l)/q2(l) se diferencia de 'propcarga'
!     pues vimos que la carga vista por el anillo es
!     aprox. es ese proporcion de la carga vista por la olla

              x1=q1(l)/q(l)
              x2=q2(l)/q(l)

      penal3(l)=3.*abs(x1-propcarga*x2)
             
**********CALCULO DE TERCERA PENALIZACION ********
!     La variable 'penal3' cuantifica cuanto estan separados
!     los picos de la olla y del anillo
!     Vimos que la distancia p1(l)-u1(l) deberia ser
!     la proporcion 'propancho' de p2(l)-u1(l) aproximadamente

              x1=real(p1(l)-u1(l))
              x2=real(p2(l)-u1(l))
              y1=2.*x1/(x1/propancho+x2)
              y2=2.*x2/(x1/propancho+x2)              

       penal4(l)=3.*abs(y1-propancho*y2)

       
**********CALCULO DE CUARTA PENALIZACION ********
!     La variable 'penal4' calcula la proporcion de senial que esta entre
!     los limites 0.02 y -0.02 sin ser cero
!     esto es porque cuanto mas pequenia es la senial, hay mas peligro
!     de que sea un mal pulso

       w1prom=0.
       w2prom=0.
       do iii=0,4*nn
       w1prom=w1prom+w1(iii,l)/real(4*nn+1)
       w2prom=w2prom+w2(iii,l)/real(4*nn+1)          
       enddo
       
        cc1=0
        cc2=0
        do iii=0,4*nn
           if (abs(w1(iii,l)-w1prom).gt.0.) then
              cc1=cc1+1
              if (abs(w1(iii,l)-w1prom).le.0.02) then
                 cc2=cc2+1
              endif
           endif
           if (abs(w2(iii,l)-w2prom).gt.0.) then
              cc1=cc1+1
              if (abs(w2(iii,l)-w2prom).le.0.02) then
                 cc2=cc2+1
              endif
           endif
         enddo
         penal5(l)=(real(cc2)/real(cc1))**2. 
        
**********SUMA DE LAS CUATRO PENALIZACIONES ********           
        
         penal(l)=penal1(l)+penal2(l)+penal3(l)+penal4(l)+penal5(l)
         

         
***********
         endif
      enddo
   

         
************************************************
***** CONTADOR DE PULSOS BUENOS ****************      
************************************************
      cont=0
      do l=1,pul(s)
         if (indx(l).eq.1) then
            cont=cont+1            
         endif
      enddo

      contt=contt+cont
      write(*,*) s,pul(s),cont,contt

************************************************
***** GRABADO DE PULSOS (INTEGRAL) PARA OBSERVAR
************************************************      
!     estan grabados en orden creciente del la variable 'penal'

      cont=0
      do i=0,4000   
      do l=1,pul(s)
         if (indx(l).eq.1) then
            if ((penal(l).ge.real(i)/100.).and.
     &          (penal(l).lt.real(i+1)/100.)) then
            cont=cont+1
           do w=0,4*nn
             write(30,*) cont+real(w)/real(4*nn+1),2.*1000./rate*i1(w,l)
     &                                            ,2.*1000./rate*i2(w,l)
          enddo
          endif
          endif
       enddo
      enddo
      

************************************************
***** GRABADO DE PULSOS (CORRIENTE) PARA OBSERVAR
************************************************ 
!     estan grabados en orden creciente del la variable 'penal'
      
      write(30,*)
      cont=0
      do i=0,4000
         do l=1,pul(s)
         if (indx(l).eq.1) then
            if ((penal(l).ge.real(i)/100.).and.
     &          (penal(l).lt.real(i+1)/100.)) then
               cont=cont+1
               orden(l)=cont
            do w=0,4*nn
               write(30,*) cont+real(w)/real(4*nn+1),w1(w,l),w2(w,l)
            enddo
          endif
            endif
       enddo
       
      enddo

************************************************
***** GRABADO DE PULSOS (MODELO) PARA OBSERVAR
************************************************      
!     estan grabados en orden creciente del la variable 'penal'

      write(30,*)
      cont=0
      do i=0,4000
         do l=1,pul(s)
         if (indx(l).eq.1) then
            if ((penal(l).ge.real(i)/100.).and.
     &          (penal(l).lt.real(i+1)/100.)) then
             cont=cont+1  
            do w=0,4*nn
               write(30,*) cont+real(w)/real(4*nn+1)
     &                    ,a1(w,l),b1(w,l),a2(w,l),penal(l)
            enddo
         endif
         endif
       enddo     
      enddo
      

************************************************
***** GRABADO DE PICOS DE LOS PULSOS PARA OBSERVAR
************************************************      
!     estan grabados en orden creciente del la variable 'penal'
      
      write(30,*)
      cont=0
      do i=0,4000
         do l=1,pul(s)
         if (indx(l).eq.1) then
            if ((penal(l).ge.real(i)/100.).and.
     &          (penal(l).lt.real(i+1)/100.)) then
             cont=cont+1  
         write(30,*) cont+real(p1(l)-u1(l))/real(4*nn+1),q1(l)      
         write(30,*) cont+real(p2(l)-u1(l))/real(4*nn+1),q2(l)
            endif
            endif
       enddo
        enddo
      
    
          close(20)
          close(30)     
         

    
****************************************************
***** CALCULO DE DIAMETROS                  ********
****************************************************
      
      write(120,*) cont

**********CAlCULO DE LA VELOCIDAD DE CAIDA ********
         
      do l=1,pul(s)
         if (indx(l).eq.1) then
          
            v(l)=separacion_anillo_olla/(t(p2(l))-t(p1(l)))*rate

            if (v(l).ge.9.0238480018) then
               d(l)=5.81
            endif
            
            if  (v(l).le.0.7125) then
               d(l)=0.175
            endif

            if ((v(l).gt.0.71252).and.(v(l).lt.9.0238480018)) then

            do k=2,1128
                  
               if ((v(l).gt.vc(k-1)).and.(v(l).le.vc(k))) then
                  d(l)=dc(k)
               endif
            enddo
            
         endif
         
****************************************************
***** GRABADO DE TODOS LOS DATOS            ********
****************************************************
         
        write(120,*) s,orden(l),u1(l),u1(l)+t1(s),q1(l),q2(l),q(l)
     &   ,d(l),v(l),p1(l)-u1(l),p2(l)-u1(l),
     &        penal1b(l),penal1(l),penal2(l),penal3(l)
     &       ,penal4(l),penal5(l),penal(l)
    
            endif
      enddo
  
      enddo

      end
      
      
