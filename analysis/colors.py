import numpy as np
Ioncolors = {"H":"darkred","He":"grey","C":"lawngreen","Fe":"red","Mg24":"slateblue","Si28":"sandybrown","N14":"mistyrose","pi+":"lightcoral","pi-":"rosybrown","deuteron":"olivedrab","He3":"lime","e-":"blue","kaon+":"silver","kaon0L":"gainsboro","triton":"brown","kaon0S":"fuchsia","lambda":"salmon","B11":"darkviolet","Ne21":"darkorchid","kaon-":"seagreen","O":"aqua","sigma+":"peru","Li6":"forestgreen","mu+":"teal","mu-":"lightblue","anti_proton":"tomato","sigma-":"saddlebrown","anti_neutron":"royalblue","xi0":"olivedrab","Others":"cornflowerblue","Total":"black"}

color_thick = [
    "#FF0000",  # Bright Red
    "#FF3300",  # Reddish-Orange
    "#FF6600",  # Orange
    "#FF9933",  # Yellow-Orange
    "#FFCC00",  # Golden Yellow
    "#FFFF00",  # Yellow
    "#CCFF00",  # Yellow-Green
    "#99FF33",  # Lime Green
    "#33FF66",  # Bright Green
    "#00FF99",  # Greenish-Cyan
    "#00FFFF",  # Cyan
    "#3399FF",  # Sky Blue
    "#6633FF",  # Blue-Violet
    "#9900FF",  # Violet
    "#CC00FF",  # Bright Magenta
    "#FF00FF"   # Purple
]

def get_thick_color(value, max_value=200):
    """
    Get a color in RGB format based on an input value, varying linearly from 0 to max_value,
    following the rainbow pattern.

    Args:
        value (float): The input value, between 0 and max_value.
        max_value (float): The maximum possible value.
    
    Returns:
        Tuple[float, float, float]: A color in RGB format (values between 0 and 1).
    """
    if max_value <= 0:
        raise ValueError("max_value must be greater than 0")
    
    # Clamp value to the range [0, max_value]
    value = np.clip(value, 0, max_value)
    
    # Define the base rainbow colors in RGB (normalized to [0, 1])
    rainbow_colors = [
        (1, 0, 0),   # Red
        (1, 0.5, 0), # Orange
        (1, 1, 0),   # Yellow
        (0, 1, 0),   # Green
        (0, 0, 1),   # Blue
        (0.29, 0, 0.51), # Indigo
        (0.56, 0, 1) # Violet
    ]
    
    # Normalize value to a range [0, 1]
    normalized_value = value / max_value
    
    # Map normalized value to the range of indices in rainbow_colors
    color_index = normalized_value * (len(rainbow_colors) - 1)
    lower_index = int(np.floor(color_index))
    upper_index = int(np.ceil(color_index))
    
    # Interpolate between the two surrounding colors
    t = color_index - lower_index  # Fractional part
    color = tuple(
        (1 - t) * rainbow_colors[lower_index][i] + t * rainbow_colors[upper_index][i]
        for i in range(3)
    )
    
    return color

