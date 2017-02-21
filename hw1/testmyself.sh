#!/bin/sh
PATH="."
count=0;
for ((i=0;i<10;i++))
{
	./beargit commit -m "GO BEARS!script $count";
	let count++;
}
