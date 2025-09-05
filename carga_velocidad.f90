program fortran_example
    implicit none

    integer :: ios, step, id, last_id
    real :: time, sensor1, sensor2, integral1, integral2
    real :: a1, a2, b1, q1, q2, v, diameter
    real :: sum_sq1, sum_sq2, charge_penalty, width_penalty, noise_penalty
    real :: sum_penalty, q
    character(len=512) :: header

    open(unit=10, file='drops.dat', status='old')
    read(10, '(A)') header

    open(unit=20, file='carga_velocidad.dat', status='replace')
    write(20, *) 'step,', 'q1,', 'q2,', 'q,', 'v,', 'diameter,', 'sum_of_penalties'

    last_id = -1
    do
        read(10, *, iostat=ios) time, step, sensor1, sensor2, integral1, integral2, &
            a1, a2, b1, q1, q2, v, diameter, sum_sq1, sum_sq2, charge_penalty, &
            width_penalty, noise_penalty, sum_penalty, id
        if (ios /= 0) exit

        if (id /= last_id) then
            q = (q1 / 0.87 + q2) / 2.
            write(20, *) step, q1, q2, q, v, diameter, sum_penalty
            last_id = id
        end if
    end do

    close(10)
    close(20)
end program fortran_example

