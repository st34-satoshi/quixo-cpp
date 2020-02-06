# each number (o number and x number) to table
# ex)
# o+x = 4
# o, x = 0, 4 = 1810
# o, x = 1, 3 = 1612
# o, x = 2, 2 = 1442
# o, x = 3, 1 = 2180
# o, x = 4, 0 = 1810


import matplotlib.pyplot as plt
import math

data_file_name = "tt"


def number_type(number):
    if number == 0:
        return 0
    return int(math.log(number, 10)) + 1


def read_data(file_name, cells):
    with open(file_name) as f:
        data_list = f.readlines()
        for data in data_list:
            if data[:2] == "o,":
                line_data = data.replace(" ", "").split("=")
                ox = line_data[1].split(",")
                o_number = int(ox[0])
                x_number = int(ox[1])
                number = int(line_data[2])
                cells[o_number][x_number] = number
    return cells


# Prepare table
s = 4
board_size = s*s
columns = ["o="+str(i) for i in range(board_size+1)]
rows = ["x="+str(i) for i in range(board_size+1)]
cell_text = [[0 for j in range(board_size+1)] for i in range(board_size+1)]

# read data from file
cell_text = read_data(data_file_name, cell_text)
# for i in range(board_size+1):
#     for j in range(board_size+1):
#         print(i, j, cell_text[i][j] / max_number)

# color
cm_name = 'pink_r'
cm = plt.get_cmap(cm_name)
# for i in range(10):
#     print(i, cm(i))
# c1 = [cm(i*50) for i in range(4)]
# colors = [[cm(cell_text[i][j] * 200 // max_number) for j in range(board_size+1)] for i in range(board_size+1)]
colors = [[cm((number_type(cell_text[i][j])*40)) for j in range(board_size+1)] for i in range(board_size+1)]
# colors = [[cm(j*(board_size+1)+i) for j in range(board_size+1)] for i in range(board_size+1)]
# cell_text = [[j*(board_size+1)+i for j in range(board_size+1)] for i in range(board_size+1)]
# cell_text = [[(cell_text[i][j] / max_number) for j in range(board_size+1)] for i in range(board_size+1)]

fig, ax = plt.subplots()
# ax.axis('tight')
ax.axis('off')
the_table = ax.table(cellText=cell_text,
                     cellColours=colors,
                     colLabels=columns,
                     rowLabels=rows,
                     loc='center')
plt.title("not reachable states number")
plt.xlabel("o Number")
# plt.legend()
plt.show()
# plt.savefig("color-table")
