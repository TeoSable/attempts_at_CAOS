#!/usr/bin/env bash

# Ввод матрицы СЛАУ
declare -A matrix
i=1
IFS=$'\n'
for line in $(cat $1)
do
  IFS=","
  j=1
  for val in $line
  do
    matrix[$i,$j]=$val
    j=$(($j+1))
  done
  i=$(($i+1))
  IFS=$'\n'
done

# Размеры матрицы
N=$((i-1))
#echo "$N"

# Печать матрицы
function printMatrix() {
for ((k=1;k<=N;k++)) do
  for ((l=1;l<=N+1;l++)) do
    printf " ${matrix[$k,$l]}"
  done
  echo
done
echo
}

# Метод Гаусса
#printMatrix
for ((i=1;i<=N;i++))
do
  # Если i-й элемент i-й строки нулевой, ищем первую строку с ненулевым i-м элементом и меняем её поэлементно с i-й
    f=$i
    if [[ $(echo "${matrix[$f,$i]} == 0" | bc) -eq 1 ]]; then
    while [[ $(echo "${matrix[$f,$i]} == 0" | bc) -eq 1 ]]
    do
      if ((f > N))
      then
        break
      else
        f=$((f+1))
      fi
    done
    for ((j=1;j<=N+1;j++))
    do
      tmp=${matrix[$i,$j]}
      matrix[$i,$j]=${matrix[$f,$j]}
      matrix[$f,$j]=$tmp
    done
  fi

  # Делим все ненулевые элементы i-й строки на i-й элемент
  d=${matrix[$i,$i]}
  for ((j=i;j<=N+1;j++))
  do
    matrix[$i,$j]=$(echo "${matrix[$i,$j]} / $d" | bc -l)
    #printMatrix
  done

  # Вычитаем из каждой строки, кроме i-й, i-ю, умноженную на i-й элемент соответствующей строки, поэлементно
  for ((m=1;m<=N;m++))
  do
    if [[ $m -ne $i ]]
    then
      mult=${matrix[$m,$i]}
      for ((j=i;j<=N+1;j++))
      do
        matrix[$m,$j]=$(echo "${matrix[$m,$j]} - ${matrix[$i,$j]} * $mult" | bc -l)
      done
      #printMatrix
    fi
  done
done

# Вывод решения на экран
for ((i=1;i<=N;i++))
do
  echo "${matrix[$i,$((N+1))]}"
done
