# ./run.sh > sampling_time.txt &
#!/bin/bash
# Loop through the first set of parameters
echo "Start running the main program"
for i in 64 128 256; do
    # Nested loop for the second set of parameters
    for j in 64 128 256 512 1024 2048; do
        for k in {1..3}
        do
            ./main $i $j
        done
        echo "Samping num: $i; mini batch_size: $j"
        echo "================================"
        echo -e "\n\n"
    done
done
echo "End running the main program"