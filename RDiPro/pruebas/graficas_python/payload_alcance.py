"""
========
Barchart
========

A bar plot with errorbars and height labels on individual bars
"""
import numpy as np
import matplotlib.pyplot as plt

N = 4
rdipro_means = (2, 4, 4, 4)
rdipro_std = (0.01, 0.01, 0.01, 0.01)

ind = np.arange(N)  # the x locations for the groups
width = 0.35       # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(ind, rdipro_means, width, color='r', yerr=rdipro_std)

netflood_means = (4, 3, 4, 4)
netflood_std = (0.01, 0.01, 0.01, 0.01)
rects2 = ax.bar(ind + width, netflood_means, width, color='y', yerr=netflood_std)

# add some text for labels, title and axes ticks
ax.set_ylabel('N. de nodos que recibieron el mensaje')
ax.set_xlabel('N. de bytes')
ax.set_title('Alcance variando el payload')
ax.set_xticks(ind + width / 2)
ax.set_xticklabels(('5', '10', '15', '20'))

ax.legend((rects1[0], rects2[0]), ('RDiPro', 'Flooding'))


def autolabel(rects):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                '%d' % int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)

#plt.show()
plt.savefig('payload_alcance.png', dpi = 300)
plt.close()
