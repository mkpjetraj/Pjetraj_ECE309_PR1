#!/bin/bash
            # simple bash script to automate testing of the 'harness' program

            # use printf to generate the two lines of input, separated by newlines,
            # and pipe them into the harness program's stdin
            printf "hello\nexit\n" | ./harness
