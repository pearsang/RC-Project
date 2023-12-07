make clean
make fmt
make user

# do loop to run all tests
#for i in {1..29}
#do
#    echo "Running test $i"
#    ./user < scripts/script_0$i.txt
#done

# run test 10
#./user < scripts/script_09.txt


progress-bar() {
  local duration=${1}

    already_done() { for ((done=0; done<$elapsed; done++)); do printf "▇"; done }
    remaining() { for ((remain=$elapsed; remain<$duration; remain++)); do printf " "; done }
    percentage() { printf "| %s%%" $(( (($elapsed)*100)/($duration)*100/100 )); }
    clean_line() { printf "\r"; }

  for (( elapsed=1; elapsed<=$duration; elapsed++ )); do
      already_done; remaining; percentage
      sleep 1
      clean_line
  done
  clean_line
}

# make clean