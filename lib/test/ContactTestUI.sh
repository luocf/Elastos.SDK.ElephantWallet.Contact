#!/bin/bash

set -o nounset
set -o errexit

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
CACHE_DIR="/tmp/elastos.sdk.contact/test";
FIFO_FILE="$CACHE_DIR/cmdpipe-$RANDOM";
SHCFG_FILE="$CACHE_DIR/shell.cfg";

CMD_LIST=(
    'h'
    'p'
    'c'
    'd'
    'u'
    'a'
    's'
)
CMD_LIST=(
    'h' 'help'
    'p' 'print-info'
    'c' 'print-carrier'
    'd' 'print-cachedinfo'
    'u' 'upload-info'
    'a' 'add-friend'
    's' 'send-message'
)

EXEC_USER="$SCRIPT_DIR/ContactTest $FIFO_FILE";
EXEC_FRIEND="$SCRIPT_DIR/ContactTest";

mkdir -p $(dirname "$SHCFG_FILE");
echo ""                                     >  "$SHCFG_FILE";
echo "alias tmux='$(which tmux)';"          >> "$SHCFG_FILE";
echo "alias clear='$(which clear)';"        >> "$SHCFG_FILE";
echo "alias exit='tmux kill-session';"      >> "$SHCFG_FILE";
echo "alias quit='exit';alias q='quit';"    >> "$SHCFG_FILE";
for cmd in "${CMD_LIST[@]}"; do
echo "alias $cmd='echo $cmd > $FIFO_FILE';" >> "$SHCFG_FILE";
done
echo "export PATH=;"                        >> "$SHCFG_FILE";
echo "$(which clear)"                       >> "$SHCFG_FILE";

mkdir -p $(dirname "$FIFO_FILE");
mkfifo "$FIFO_FILE";

tmux \
    set-window-option -g mouse on\; \
    set-window-option -g mode-keys vi\; \
    bind-key -n ESCAPE copy-mode\; \
    bind-key -T copy-mode-vi 'v' send -X begin-selection\; \
    bind-key -T copy-mode-vi 'y' send -X copy-pipe-and-cancel 'pbcopy'\; \
    bind-key -T copy-mode-vi 'p' run "pbpaste | tmux load-buffer - && tmux paste-buffer && tmux send-keys ESCAPE"\; \
    set -sg escape-time 0 \; \
    new-session \; \
    send-keys "source $SHCFG_FILE && $EXEC_USER" Enter\; \
    split-window -v -p 15\; \
    select-pane -t 0\; \
    split-window -h\; \
    send-keys "source $SHCFG_FILE && $EXEC_FRIEND" Enter\; \
    select-pane -t 2\; \
    send-keys "source $SHCFG_FILE" Enter
