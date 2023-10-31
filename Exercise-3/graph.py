import matplotlib.pyplot as plt
import numpy as np

fig, ax = plt.subplots(1, 3)

def plot(ax, x, y, title):
	ax.plot(x, y)
	ax.set_title(title)
	ax.set_ylabel("Conexões estabelicidas")
	ax.set_xlabel("Tamanho do backlog")

plot(ax[0], np.linspace(0, 1, 10), np.linspace(0, 0.5, 10), "Requisições simultâneas: 5")
plot(ax[1], np.linspace(0, 1, 10), np.linspace(0, 0.5, 10), "Requisições simultâneas: 10")
plot(ax[2], np.linspace(0, 1, 10), np.linspace(0, 0.5, 10), "Requisições simultâneas: 15")


plt.suptitle("Diferentes quantidades de requisições e como o tamanho do backlog afeta isso")
plt.show()