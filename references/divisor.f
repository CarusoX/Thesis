      implicit none
      integer i,mm
      parameter(mm=19910000)

      real*8 t,v1,v2,v3,v4

      
************************************************
            
      open(unit=20, file='23-11-21-01-14-21-A3-A2menos_ampl.lvm')
      open(unit=30, file='23-11-21-01-14-21-A3.lvm')
      open(unit=40, file='23-11-21-01-14-21-A2menos_ampl.lvm')

      do i=1,mm
         read(20,*) t,v1,v2,v3,v4
         write(30,*) t,v1,v2
         write(40,*) t,v3,v4         
      enddo

      end

