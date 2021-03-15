declare -ra VALID_TARGETS=(
  retrofw
  retrofw-hellfire
  rg350
  rg350-hellfire
  gkd350h
  gkd350h-hellfire
)

usage_target() {
  echo -n "	target: target platform:"
  printf " %s" "${VALID_TARGETS[@]}"
  echo
}

check_target() {
  if [[ $# -eq 0 ]] || [[ -z $1 ]]; then
    echo "Error: target is missing"
    return 1
  fi

  for target in "${VALID_TARGETS[@]}"; do
    if [[ $target == $1 ]]; then
      return 0
    fi
  done
  echo "Error: invalid target"
  return 1
}
