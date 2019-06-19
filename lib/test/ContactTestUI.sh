#!/bin/bash

set -o nounset
set -o errexit

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
CACHE_DIR="/tmp/elastos.sdk.contact/test";
FIFO_FILE="$CACHE_DIR/cmdpipe-$RANDOM";

CMD_LIST=(
    'h' 'help'
    'p' 'print-info'
    'c' 'print-carrier'
    'a' 'add-friend'
    's' 'send-message'
)


EXEC_USER="$SCRIPT_DIR/ContactTest $FIFO_FILE";
EXEC_FRIEND="$SCRIPT_DIR/ContactTest";
EXEC_CMD=;
EXEC_CMD+="alias tmux='$(which tmux)';";
EXEC_CMD+="alias clear='$(which clear)';";
EXEC_CMD+="alias exit='tmux kill-session';";
EXEC_CMD+="alias quit='exit';alias q='quit';";
for cmd in "${CMD_LIST[@]}"; do
    EXEC_CMD+="alias $cmd='echo $cmd > $FIFO_FILE';";
done
EXEC_CMD+="export PATH=;";
EXEC_CMD+="$(which clear)";


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
    send-keys "$EXEC_CMD && $EXEC_USER" Enter\; \
    split-window -v -p 20\; \
    select-pane -t 0\; \
    split-window -h\; \
    send-keys "$EXEC_CMD && $EXEC_FRIEND" Enter\; \
    select-pane -t 2\; \
    send-keys "$EXEC_CMD" Enter
