program fortran_example
    implicit none

    integer :: ios, step, id, last_id
    integer :: total_gotas, total_pasos, primer_paso_primera_gota, primer_paso_ultima_gota
    real :: time, sensor1, sensor2, integral1, integral2
    real :: a1, a2, b1, q1, q2, v, diameter
    real :: sum_sq1, sum_sq2, charge_penalty, width_penalty, noise_penalty
    real :: sum_penalty, q
    real, parameter :: charge_multiplier = 5.0
    character(len=512) :: header

    open(unit=10, file='drops.dat', status='old')
    read(10, '(A)') header

    ! First pass: collect statistics
    total_gotas = 0
    total_pasos = 0
    primer_paso_primera_gota = -1
    primer_paso_ultima_gota = -1
    last_id = -1
    
    do
        read(10, *, iostat=ios) time, step, sensor1, sensor2, integral1, integral2, &
            a1, a2, b1, q1, q2, v, diameter, sum_sq1, sum_sq2, charge_penalty, &
            width_penalty, noise_penalty, sum_penalty, id
        if (ios /= 0) exit
        
        total_pasos = total_pasos + 1
        
        if (id /= last_id) then
            total_gotas = total_gotas + 1
            if (primer_paso_primera_gota == -1) then
                primer_paso_primera_gota = step
            end if
            primer_paso_ultima_gota = step
            last_id = id
        end if
    end do
    
    ! Rewind file for second pass
    rewind(10)
    read(10, '(A)') header

    open(unit=20, file='carga_velocidad.dat', status='replace')
    write(20, *) total_gotas, total_pasos, primer_paso_primera_gota, primer_paso_ultima_gota

    last_id = -1
    do
        read(10, *, iostat=ios) time, step, sensor1, sensor2, integral1, integral2, &
            a1, a2, b1, q1, q2, v, diameter, sum_sq1, sum_sq2, charge_penalty, &
            width_penalty, noise_penalty, sum_penalty, id
        if (ios /= 0) exit

        if (id /= last_id) then
            q = (q1 / 0.87 + q2) / 2.
            write(20, *) step, q1 * charge_multiplier, q2 * charge_multiplier, q * charge_multiplier, v, diameter, sum_penalty
            last_id = id
        end if
    end do

    close(10)
    close(20)
end program fortran_example

