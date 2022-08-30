import numpy as np
import matplotlib.pyplot as plt
import sympy as sym

k, tau, Kc = sym.symbols('k, tau, Kc', Real=True)
t = sym.Symbol("t", Real=True)

ti = np.arange(0, 650, 0.1)
x = (k/tau)*sym.exp(-(t/tau))
x2 = 20*sym.exp(-((110*t)/(0.48*Kc)))
c = 1 - np.exp(-ti)
plt.plot(ti, c)
plt.title(r'c(t) = $'+str(sym.latex(1 - x))+'$')
plt.xlim(0, 10)
plt.grid()
plt.show()

fig, ax = plt.subplots(1, 3)
x1 = 20*(1 - np.exp(-ti/(110/1*0.48)))
x10 = 20*(1 - np.exp(-ti/(110/10*0.48)))
x100 = 20*(1 - np.exp(-ti/(110/100*0.48)))
ax[0].plot(ti, x1, label = 'Kc = 1')
ax[0].grid()
ax[0].legend()
ax[1].plot(ti, x10, label = 'Kc = 10')
ax[1].set_title(r'c(t) = $'+str(sym.latex(x2))+'$')
ax[1].grid()
ax[1].legend()
ax[1].set_xlim(0, 60)
ax[2].plot(ti, x100, label = 'Kc = 100')
ax[2].set_xlim(0, 20)
ax[2].legend()
ax[2].grid()
plt.show()
