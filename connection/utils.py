import os
from datetime import datetimes


def check_dir(path='data'):
    if not os.path.isdir(path):
        print(f'path: "{path}" does not exists! Creating a new one!')
        os.mkdir(path)
    else:
        print(f'path: "{path}" exists!')


def get_time():
    now = datetime.now()
    t = now.strftime("%d::%m::%Y %H:%M:%S")
    return t


def save_fig(plt, path, label=None):
    if not label:
        label = get_time()

    plt.savefig(f'{path}/{path}-{label}.png')
    fig, axes = plt.subplots(nrows=3, ncols=2, sharex=True,
                             sharey=True, figsize=(16, 8))

    return fig, axes


if __name__ == '__main__':
    check_dir('data')
    check_dir('image')
