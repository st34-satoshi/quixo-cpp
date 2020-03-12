from scipy.special import comb


def read_file(file_name):
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
            print(data)

            if i % 6 == 1:
                x, o = xo(data)
                print("x,o = ", x, o)
                p_win = 0
                p_loss = 0
                p_draw = 0
                print("comb = ", comb(25, x), comb(25-x, o))
                co = comb(25, x) * comb(25 - x, o)
                print("co = ", co)
            if i % 6 in [2, 3, 4]:
                numb = deal_line(data)
                print(numb)
                print("i=", i)
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
                print("present ", p_win, p_loss, p_draw)
                if co != (p_draw+p_loss+p_win):
                    print("Error")
                    print("o = ", o, ", x=", x)
                    exit()
    print("total win = ", total_win)
    print("total loss = ", total_loss)
    print("total draw = ", total_draw)
    print(total_win+total_loss+total_draw)


def deal_line(l):
    l_list = l.split("= ")
    n = l_list[1]
    return int(n)


def xo(l):
    l_list = l.split(",")
    x = l_list[1].split("= ")[1]
    o = l_list[2].split("= ")[1]
    return int(x), int(o)


if __name__ == '__main__':
    f = "result-5by5-activeIsO.txt"
    read_file(f)
