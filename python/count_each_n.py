from scipy.special import comb
import matplotlib.pyplot as plt


def read_file(file_name):
    # o number, x number, W,L,D
    ox_values = [[[0 for _ in range(3)] for _ in range(25+1)] for _ in range(25+1)]
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
            if i % 6 == 1:
                x, o = xo(data)
                p_win = 0
                p_loss = 0
                p_draw = 0
                co = comb(25, x) * comb(25 - x, o)
            if i % 6 in [2, 3, 4]:
                numb = deal_line(data)
                if i % 6 == 2:
                    p_win += numb
                    # print(total_win)
                elif i % 6 == 3:
                    p_loss += numb
                else:
                    p_draw += numb

            if i % 6 == 5:
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


def each_n(ox_values):
    total_values = [[0 for _ in range(4)] for _ in range(25+1)]  # w,l,d, total
    for n in range(25+1):
        for o in range(n+1):
            x = n - o
            if not (o == x or o == x-1):
                continue
            for i in range(3):
                total_values[n][i] += ox_values[o][x][i]
        total = 0
        for i in range(3):
            total += total_values[n][i]
        total_values[n][3] = total
        # print("///////////// total = ", n)
        # print("win = ", total_values[n][0] / total)
        # print("loss = ", total_values[n][1] / total)
        # print("draw = ", total_values[n][2] / total)
    return total_values


def plot_WLD_per_total(total_values):
    n = [i for i in range(25+1)]
    for i in range(25+1):
        x = i // 2
        o = i - x
        n[i] = "("+str(x)+","+str(o)+")"
    win = [total_values[i][0] / total_values[i][3] for i in range(25+1)]
    loss = [total_values[i][1] / total_values[i][3] for i in range(25+1)]
    draw = [total_values[i][2] / total_values[i][3] for i in range(25+1)]
    plt.figure(figsize=(8, 8))
    plt.plot(n, win, label="Win")
    plt.plot(n, loss, label="Loss")
    plt.plot(n, draw, label="Draw")
    plt.ylabel("% of Win/Loss/Draw states number")
    plt.xlabel("the number of  tiles (x, o)")
    plt.legend()
    plt.xticks(rotation=75)
    plt.savefig("WLDPerN")
    # plt.show()


def plot_WLD_per_total_bar(total_values):
    left = [i for i in range(25+1)]
    label = [i for i in range(25+1)]
    for i in range(25+1):
        x = i // 2
        o = i - x
        label[i] = "("+str(x)+","+str(o)+")"
    win = [total_values[i][0] / total_values[i][3] * 100.0 for i in range(25+1)]
    loss = [total_values[i][1] / total_values[i][3] * 100.0 for i in range(25+1)]
    draw = [total_values[i][2] / total_values[i][3] * 100.0 for i in range(25+1)]
    win_and_loss = [win[i]+loss[i] for i in range(25+1)]
    plt.figure(figsize=(8, 9))
    p_win = plt.bar(left, win, tick_label=label, align="center")
    p_loss = plt.bar(left, loss, bottom=win, tick_label=label, align="center")
    p_draw = plt.bar(left, draw, bottom=win_and_loss, tick_label=label, align="center")

    fontsize = 22
    plt.rcParams["font.size"] = fontsize
    plt.ylabel("% of Win/Loss/Draw states number", fontsize=fontsize)
    plt.xlabel("the number of  tiles (x, o)", fontsize=fontsize)

    plt.legend((p_draw[0], p_loss[0], p_win[0]), ("Draw", "Loss", "Win"), fontsize=fontsize)
    plt.xticks(rotation=75)
    plt.savefig("WLD-percent-bar")
    # plt.show()


if __name__ == '__main__':
    f = "result-5by5-activeIsO.txt"
    ox_v = read_file(f)
    # print(ox_v)
    total_v = each_n(ox_v)
    plot_WLD_per_total_bar(total_v)
