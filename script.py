def model(u, p, q, v, nn):

    a, b = [], []

    if 2 * (p - u) < 4 * nn:
        for j in range(2 * (p - u)):
            a.push(30.6 * q / (30.6 ** (1 / 1.7) + (v / 50.7)**2 * (j - p + u) ** 2) ** 1.7)
