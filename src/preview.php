<svg width="200" height="150">
  <!-- Draw Arduino board -->
  <rect x="25" y="25" width="150" height="100" rx="10" fill="#008cba"/>
  <!-- Draw switch -->
  <rect x="50" y="50" width="30" height="30" fill="#fff"/>
  <rect x="60" y="60" width="10" height="10" fill="#008cba"/>
  <!-- Draw LED -->
  <rect x="120" y="50" width="30" height="30" fill="#fff"/>
  <circle cx="135" cy="65" r="10" fill="#008cba"/>
  <!-- Connect switch to D2 pin -->
  <line x1="35" y1="65" x2="50" y2="65" stroke="#fff" stroke-width="2"/>
  <line x1="50" y1="65" x2="50" y2="50" stroke="#fff" stroke-width="2"/>
  <line x1="50" y1="50" x2="75" y2="50" stroke="#fff" stroke-width="2"/>
  <line x1="75" y1="50" x2="75" y2="75" stroke="#fff" stroke-width="2"/>
  <line x1="75" y1="75" x2="85" y2="75" stroke="#fff" stroke-width="2"/>
  <!-- Connect LED to D13 pin -->
  <line x1="150" y1="65" x2="120" y2="65" stroke="#fff" stroke-width="2"/>
  <line x1="135" y1="50" x2="135" y2="80" stroke="#fff" stroke-width="2"/>
</svg>
