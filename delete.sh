#!/bin/bash

echo "🔪 Deleting all nixbld* users..."

# Find all users starting with nixbld
for user in $(getent passwd | cut -d: -f1 | grep '^nixbld'); do
  echo "→ Deleting user: $user"
  sudo userdel -f "$user" 2>/dev/null || echo "⚠️ Failed to delete $user"
done

# Delete nixbld group if it exists
if getent group nixbld >/dev/null; then
  echo "→ Deleting group: nixbld"
  sudo groupdel nixbld
fi

echo "✅ All nixbld users and group removed."

