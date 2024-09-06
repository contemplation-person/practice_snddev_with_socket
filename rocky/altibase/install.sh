#! /bin/bash

wget http://support.altibase.com/fileDownload.do?gubun=etc&pro_ver=44&cate=2&os=LINUX-X86&p_bit=64bit 
yum install ncurses-compat-libs
tar -xzvf ./EMG_DB.tar.gz
#./run_is.sh && ./run_il_in.sh && ./run_is_refresh_mview.sh && ./run_is_index.sh && ./run_is_fk.sh && ./run_is_repl.sh && ./run_is_job.sh   
