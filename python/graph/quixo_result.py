# download result data file from Google Drive
# !curl -L -o result.txt "https://drive.google.com/uc?export=download&id=1t2Z5cXvWddld6j_cdfbSlZXy7rObGzJU"
"""O is next to play"""
"""read result file"""
from scipy.special import comb
import matplotlib.pyplot as plt


def read_file(file_name):
    # return ox_values
    # o number, x number, W,L,D
    ox_values = [[[0 for _ in range(3)] for _ in range(25+1)] for _ in range(25+1)]  # the number of W/LD of each O and X number. [i][j][k] i= O number, j= X number, k= number of win(0), loss(1) or draw(2)
    total_win = 0
    total_loss = 0
    total_draw = 0
    # read each line
    print("read each line")
    with open(file_name) as f:
        data_list = f.readlines()
        p_win = 0
        p_loss = 0
        p_draw = 0
        co = 0
        for i, data in enumerate(data_list):
            if i % 6 == 1:  # ex) total = 25, x = 25, o = 0
                x, o = xo(data)
                co = comb(25, x) * comb(25 - x, o) # use to check the result is correct
                p_win = 0
                p_loss = 0
                p_draw = 0
            if i % 6 in [2, 3, 4]:
                numb = deal_line(data)
                if i % 6 == 2: # ex) win states = 0
                    p_win += numb
                    # print(total_win)
                elif i % 6 == 3:  # ex) loss states = 0
                    p_loss += numb
                else:  # ex) draw states = 0
                    p_draw += numb
            if i % 6 == 5: # end of one class data
                total_win += p_win
                total_loss += p_loss
                total_draw += p_draw
                if co != (p_draw+p_loss+p_win):
                    print("Error")
                    print("o = ", o, ", x=", x)
                    exit()
                ox_values[o][x][0] = p_win
                ox_values[o][x][1] = p_loss
                ox_values[o][x][2] = p_draw
    print("total win = ", total_win)
    print("total loss = ", total_loss)
    print("total draw = ", total_draw)
    print(total_win+total_loss+total_draw)
    return ox_values


def deal_line(l):
    l_list = l.split("= ")
    n = l_list[1]
    return int(n)


def xo(l):
    l_list = l.split(",")
    x = l_list[1].split("= ")[1]
    o = l_list[2].split("= ")[1]
    return int(x), int(o)


def each_diff(ox_values, diff):
    # only the classes |O|-|X|=diff or diff-1
    total_values = []  # w,l,d, total(w+l+d) of each n(o number + x number), o number, x number
    for n in range(25+1):
        for o in range(n+1):
            x = n - o
            if not (x-o==diff or x-o==diff-1):  # O is next to play. O <= X.
                continue
            values = [0] * 6
            for i in range(3):
                values[i] += ox_values[o][x][i]
                values[3] += ox_values[o][x][i]
            values[4] = o
            values[5] = x
            total_values.append(values)
            break
    return total_values


def plot_WLD_per_total_bar(total_values, save_file_name="WLD-percentage-bar"):
    length = len(total_values)
    left = [i for i in range(length)]
    label = [i for i in range(length)]
    for i in range(length):
        label[i] = f"({total_values[i][4]}, {total_values[i][5]})"
    win = [total_values[i][0] / total_values[i][3] * 100.0 for i in range(length)]
    loss = [total_values[i][1] / total_values[i][3] * 100.0 for i in range(length)]
    draw = [total_values[i][2] / total_values[i][3] * 100.0 for i in range(length)]
    win_and_loss = [win[i]+loss[i] for i in range(length)]
    plt.figure(figsize=(10, 10))
    p_win = plt.bar(left, win, tick_label=label, align="center")
    p_loss = plt.bar(left, loss, bottom=win, tick_label=label, align="center")
    p_draw = plt.bar(left, draw, bottom=win_and_loss, tick_label=label, align="center")

    fontsize = 22
    plt.rcParams["font.size"] = fontsize
    plt.ylabel("% of Win/Loss/Draw states number", fontsize=fontsize)
    plt.xlabel("the number of cubes (x, o)", fontsize=fontsize)

    plt.legend((p_draw[0], p_loss[0], p_win[0]), ("Draw", "Loss", "Win"), fontsize=fontsize, loc='lower left')
    plt.xticks(rotation=75, fontsize=fontsize)
    plt.savefig(save_file_name+"h.pdf", format='pdf')
    # plt.show()


if __name__ == '__main__':

    """make a graph
    """

    # read a result
    file_name = "result-5by5-activeisO.txt"
    ox_v = read_file(file_name)
    total_v = each_diff(ox_v,1)
    # make/plot graph
    plot_WLD_per_total_bar(total_v, "diff-1")

    # total_v = each_diff(ox_v, 3)  # diff is 3 or 2
    # # make/plot graph
    # plot_WLD_per_total_bar(total_v, "diff-3")

