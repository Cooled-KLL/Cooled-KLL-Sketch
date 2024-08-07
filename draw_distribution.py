import matplotlib.pyplot as plt

group_index = []
group_frequency = []

with open('distribution.txt', 'r') as file:
    group_index = [float(num) for num in file.readline().split()]
    group_frequency = [float(num) for num in file.readline().split()]

width = [group_index[i+1] - group_index[i] for i in range(len(group_index) - 1)]
centers = [group_index[i] + width[i] / 2 for i in range(len(width))]

plt.bar(centers, group_frequency, width=width, align='center')
plt.yscale('log')
plt.xlabel("Value Range")
plt.ylabel("Frequency")
plt.title("Histogram Representation of Data")
plt.grid(True)
plt.show()