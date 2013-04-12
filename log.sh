#!/bin/bash

echo -e "$(whoami): $(date) - $*\n" >> README.md
echo "Log successful..."