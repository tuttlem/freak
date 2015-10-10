
#include "fixed.h"

fixed _fixed_sin[1024] = {
             0,        402,        804,       1206,       1608,       2010, 
          2412,       2814,       3216,       3617,       4019,       4420, 
          4821,       5222,       5623,       6023,       6424,       6824, 
          7224,       7623,       8022,       8421,       8820,       9218, 
          9616,      10014,      10411,      10808,      11204,      11600, 
         11996,      12391,      12785,      13180,      13573,      13966, 
         14359,      14751,      15143,      15534,      15924,      16314, 
         16703,      17091,      17479,      17867,      18253,      18639, 
         19024,      19409,      19792,      20175,      20558,      20939, 
         21320,      21699,      22078,      22457,      22834,      23211, 
         23586,      23961,      24335,      24708,      25080,      25451, 
         25821,      26190,      26558,      26925,      27291,      27656, 
         28020,      28383,      28745,      29106,      29466,      29824, 
         30182,      30538,      30894,      31248,      31601,      31952, 
         32303,      32652,      33000,      33347,      33692,      34037, 
         34380,      34721,      35062,      35401,      35739,      36075, 
         36410,      36744,      37076,      37407,      37736,      38064, 
         38391,      38716,      39040,      39362,      39683,      40002, 
         40320,      40636,      40951,      41264,      41576,      41886, 
         42194,      42501,      42806,      43110,      43412,      43713, 
         44011,      44309,      44604,      44898,      45190,      45480, 
         45769,      46056,      46341,      46625,      46906,      47186, 
         47464,      47741,      48015,      48288,      48559,      48828, 
         49096,      49361,      49625,      49886,      50146,      50404, 
         50660,      50914,      51167,      51417,      51665,      51912, 
         52156,      52399,      52639,      52878,      53114,      53349, 
         53581,      53812,      54040,      54267,      54491,      54714, 
         54934,      55152,      55368,      55583,      55795,      56004, 
         56212,      56418,      56621,      56823,      57022,      57219, 
         57414,      57607,      57798,      57986,      58172,      58357, 
         58538,      58718,      58896,      59071,      59244,      59415, 
         59583,      59750,      59914,      60076,      60235,      60392, 
         60547,      60700,      60851,      60999,      61145,      61288, 
         61430,      61569,      61705,      61839,      61971,      62101, 
         62228,      62353,      62476,      62596,      62714,      62830, 
         62943,      63054,      63162,      63268,      63372,      63473, 
         63572,      63669,      63763,      63854,      63944,      64031, 
         64115,      64197,      64277,      64354,      64429,      64501, 
         64571,      64639,      64704,      64767,      64827,      64885, 
         64940,      64993,      65043,      65091,      65137,      65180, 
         65220,      65259,      65294,      65328,      65358,      65387, 
         65413,      65436,      65457,      65476,      65492,      65505, 
         65516,      65525,      65531,      65535,      65536,      65535, 
         65531,      65525,      65516,      65505,      65492,      65476, 
         65457,      65436,      65413,      65387,      65358,      65328, 
         65294,      65259,      65220,      65180,      65137,      65091, 
         65043,      64993,      64940,      64884,      64827,      64766, 
         64704,      64639,      64571,      64501,      64429,      64354, 
         64277,      64197,      64115,      64030,      63944,      63854, 
         63762,      63668,      63572,      63473,      63372,      63268, 
         63162,      63053,      62943,      62829,      62714,      62596, 
         62476,      62353,      62228,      62101,      61971,      61839, 
         61705,      61568,      61429,      61288,      61144,      60999, 
         60850,      60700,      60547,      60392,      60235,      60075, 
         59913,      59749,      59583,      59414,      59244,      59071, 
         58895,      58718,      58538,      58356,      58172,      57986, 
         57797,      57607,      57414,      57219,      57022,      56822, 
         56621,      56417,      56212,      56004,      55794,      55582, 
         55368,      55152,      54934,      54713,      54491,      54266, 
         54040,      53811,      53581,      53348,      53114,      52877, 
         52639,      52398,      52156,      51911,      51665,      51416, 
         51166,      50914,      50660,      50404,      50146,      49886, 
         49624,      49360,      49095,      48828,      48559,      48288, 
         48015,      47740,      47464,      47186,      46906,      46624, 
         46341,      46055,      45768,      45480,      45189,      44897, 
         44603,      44308,      44011,      43712,      43412,      43110, 
         42806,      42501,      42194,      41885,      41575,      41263, 
         40950,      40636,      40319,      40002,      39682,      39361, 
         39039,      38715,      38390,      38064,      37736,      37406, 
         37075,      36743,      36409,      36074,      35738,      35400, 
         35061,      34721,      34379,      34036,      33692,      33346, 
         32999,      32651,      32302,      31951,      31600,      31247, 
         30893,      30538,      30181,      29824,      29465,      29105, 
         28745,      28383,      28020,      27656,      27290,      26924, 
         26557,      26189,      25820,      25450,      25079,      24707, 
         24334,      23960,      23585,      23210,      22833,      22456, 
         22078,      21699,      21319,      20938,      20557,      20175, 
         19792,      19408,      19023,      18638,      18252,      17866, 
         17479,      17091,      16702,      16313,      15923,      15533, 
         15142,      14750,      14358,      13966,      13572,      13179, 
         12785,      12390,      11995,      11599,      11203,      10807, 
         10410,      10013,       9615,       9217,       8819,       8420, 
          8022,       7622,       7223,       6823,       6423,       6023, 
          5622,       5221,       4820,       4419,       4018,       3616, 
          3215,       2813,       2411,       2009,       1608,       1205, 
           803,        401,          0, 4294966894, 4294966492, 4294966090, 
    4294965688, 4294965286, 4294964884, 4294964482, 4294964080, 4294963679, 
    4294963277, 4294962876, 4294962475, 4294962074, 4294961673, 4294961273, 
    4294960872, 4294960472, 4294960073, 4294959673, 4294959274, 4294958875, 
    4294958476, 4294958078, 4294957680, 4294957282, 4294956885, 4294956488, 
    4294956092, 4294955696, 4294955300, 4294954905, 4294954511, 4294954117, 
    4294953723, 4294953330, 4294952937, 4294952545, 4294952154, 4294951763, 
    4294951372, 4294950982, 4294950593, 4294950205, 4294949817, 4294949430, 
    4294949043, 4294948657, 4294948272, 4294947888, 4294947504, 4294947121, 
    4294946739, 4294946357, 4294945977, 4294945597, 4294945218, 4294944840, 
    4294944462, 4294944086, 4294943710, 4294943335, 4294942961, 4294942589, 
    4294942217, 4294941846, 4294941475, 4294941106, 4294940738, 4294940371, 
    4294940005, 4294939640, 4294939276, 4294938913, 4294938551, 4294938190, 
    4294937830, 4294937472, 4294937114, 4294936758, 4294936403, 4294936049, 
    4294935696, 4294935344, 4294934994, 4294934644, 4294934296, 4294933949, 
    4294933604, 4294933260, 4294932917, 4294932575, 4294932235, 4294931895, 
    4294931558, 4294931221, 4294930886, 4294930553, 4294930220, 4294929889, 
    4294929560, 4294929232, 4294928905, 4294928580, 4294928256, 4294927934, 
    4294927613, 4294927294, 4294926976, 4294926660, 4294926345, 4294926032, 
    4294925721, 4294925411, 4294925102, 4294924795, 4294924490, 4294924186, 
    4294923884, 4294923584, 4294923285, 4294922988, 4294922692, 4294922399, 
    4294922107, 4294921816, 4294921527, 4294921241, 4294920955, 4294920672, 
    4294920390, 4294920110, 4294919832, 4294919556, 4294919281, 4294919008, 
    4294918737, 4294918468, 4294918201, 4294917935, 4294917672, 4294917410, 
    4294917150, 4294916892, 4294916636, 4294916382, 4294916130, 4294915880, 
    4294915631, 4294915385, 4294915140, 4294914898, 4294914657, 4294914419, 
    4294914182, 4294913948, 4294913715, 4294913485, 4294913256, 4294913030, 
    4294912805, 4294912583, 4294912363, 4294912144, 4294911928, 4294911714, 
    4294911502, 4294911292, 4294911084, 4294910879, 4294910675, 4294910474, 
    4294910274, 4294910077, 4294909882, 4294909690, 4294909499, 4294909310, 
    4294909124, 4294908940, 4294908758, 4294908578, 4294908401, 4294908226, 
    4294908053, 4294907882, 4294907713, 4294907547, 4294907383, 4294907221, 
    4294907062, 4294906904, 4294906749, 4294906596, 4294906446, 4294906298, 
    4294906152, 4294906008, 4294905867, 4294905728, 4294905592, 4294905457, 
    4294905325, 4294905196, 4294905068, 4294904943, 4294904821, 4294904701, 
    4294904583, 4294904467, 4294904354, 4294904243, 4294904135, 4294904029, 
    4294903925, 4294903824, 4294903725, 4294903628, 4294903534, 4294903442, 
    4294903353, 4294903266, 4294903182, 4294903100, 4294903020, 4294902943, 
    4294902868, 4294902796, 4294902726, 4294902658, 4294902593, 4294902530, 
    4294902470, 4294902412, 4294902357, 4294902304, 4294902254, 4294902206, 
    4294902160, 4294902117, 4294902076, 4294902038, 4294902003, 4294901969, 
    4294901938, 4294901910, 4294901884, 4294901861, 4294901840, 4294901821, 
    4294901805, 4294901792, 4294901781, 4294901772, 4294901766, 4294901762, 
    4294901761, 4294901762, 4294901766, 4294901772, 4294901781, 4294901792, 
    4294901805, 4294901821, 4294901840, 4294901861, 4294901884, 4294901910, 
    4294901939, 4294901969, 4294902003, 4294902039, 4294902077, 4294902117, 
    4294902160, 4294902206, 4294902254, 4294902304, 4294902357, 4294902413, 
    4294902471, 4294902531, 4294902593, 4294902659, 4294902726, 4294902796, 
    4294902868, 4294902943, 4294903021, 4294903100, 4294903182, 4294903267, 
    4294903354, 4294903443, 4294903535, 4294903629, 4294903725, 4294903824, 
    4294903926, 4294904029, 4294904135, 4294904244, 4294904355, 4294904468, 
    4294904583, 4294904701, 4294904822, 4294904944, 4294905069, 4294905196, 
    4294905326, 4294905458, 4294905592, 4294905729, 4294905868, 4294906009, 
    4294906153, 4294906299, 4294906447, 4294906597, 4294906750, 4294906905, 
    4294907062, 4294907222, 4294907384, 4294907548, 4294907714, 4294907883, 
    4294908054, 4294908227, 4294908402, 4294908580, 4294908759, 4294908941, 
    4294909125, 4294909312, 4294909500, 4294909691, 4294909884, 4294910079, 
    4294910276, 4294910475, 4294910676, 4294910880, 4294911086, 4294911293, 
    4294911503, 4294911715, 4294911929, 4294912146, 4294912364, 4294912584, 
    4294912807, 4294913031, 4294913258, 4294913486, 4294913717, 4294913949, 
    4294914184, 4294914420, 4294914659, 4294914899, 4294915142, 4294915386, 
    4294915633, 4294915881, 4294916132, 4294916384, 4294916638, 4294916894, 
    4294917152, 4294917412, 4294917674, 4294917937, 4294918203, 4294918470, 
    4294918739, 4294919010, 4294919283, 4294919557, 4294919834, 4294920112, 
    4294920392, 4294920674, 4294920957, 4294921242, 4294921529, 4294921818, 
    4294922108, 4294922400, 4294922694, 4294922990, 4294923287, 4294923586, 
    4294923886, 4294924188, 4294924492, 4294924797, 4294925104, 4294925412, 
    4294925723, 4294926034, 4294926347, 4294926662, 4294926978, 4294927296, 
    4294927615, 4294927936, 4294928258, 4294928582, 4294928907, 4294929234, 
    4294929562, 4294929892, 4294930222, 4294930555, 4294930888, 4294931223, 
    4294931560, 4294931898, 4294932237, 4294932577, 4294932919, 4294933262, 
    4294933606, 4294933952, 4294934298, 4294934646, 4294934996, 4294935346, 
    4294935698, 4294936051, 4294936405, 4294936760, 4294937117, 4294937474, 
    4294937833, 4294938192, 4294938553, 4294938915, 4294939278, 4294939642, 
    4294940007, 4294940373, 4294940741, 4294941109, 4294941478, 4294941848, 
    4294942219, 4294942591, 4294942964, 4294943338, 4294943712, 4294944088, 
    4294944465, 4294944842, 4294945220, 4294945599, 4294945979, 4294946360, 
    4294946741, 4294947123, 4294947506, 4294947890, 4294948274, 4294948660, 
    4294949045, 4294949432, 4294949819, 4294950207, 4294950596, 4294950985, 
    4294951375, 4294951765, 4294952156, 4294952547, 4294952940, 4294953332, 
    4294953725, 4294954119, 4294954513, 4294954908, 4294955303, 4294955698, 
    4294956094, 4294956491, 4294956888, 4294957285, 4294957682, 4294958080, 
    4294958479, 4294958877, 4294959276, 4294959676, 4294960075, 4294960475, 
    4294960875, 4294961275, 4294961676, 4294962077, 4294962478, 4294962879, 
    4294963280, 4294963681, 4294964083, 4294964485, 4294964886, 4294965288, 
    4294965690, 4294966092, 4294966494, 4294966897 };
 

fixed _fixed_cos[1024] = {
         65536,      65535,      65531,      65525,      65516,      65505, 
         65492,      65476,      65457,      65436,      65413,      65387, 
         65358,      65328,      65294,      65259,      65220,      65180, 
         65137,      65091,      65043,      64993,      64940,      64884, 
         64827,      64766,      64704,      64639,      64571,      64501, 
         64429,      64354,      64277,      64197,      64115,      64031, 
         63944,      63854,      63763,      63668,      63572,      63473, 
         63372,      63268,      63162,      63054,      62943,      62830, 
         62714,      62596,      62476,      62353,      62228,      62101, 
         61971,      61839,      61705,      61568,      61429,      61288, 
         61145,      60999,      60851,      60700,      60547,      60392, 
         60235,      60075,      59914,      59750,      59583,      59415, 
         59244,      59071,      58895,      58718,      58538,      58356, 
         58172,      57986,      57798,      57607,      57414,      57219, 
         57022,      56823,      56621,      56418,      56212,      56004, 
         55794,      55582,      55368,      55152,      54934,      54713, 
         54491,      54267,      54040,      53812,      53581,      53349, 
         53114,      52877,      52639,      52398,      52156,      51911, 
         51665,      51417,      51166,      50914,      50660,      50404, 
         50146,      49886,      49624,      49361,      49095,      48828, 
         48559,      48288,      48015,      47741,      47464,      47186, 
         46906,      46624,      46341,      46056,      45769,      45480, 
         45190,      44897,      44604,      44308,      44011,      43712, 
         43412,      43110,      42806,      42501,      42194,      41885, 
         41575,      41264,      40951,      40636,      40320,      40002, 
         39683,      39362,      39040,      38716,      38391,      38064, 
         37736,      37406,      37076,      36743,      36410,      36075, 
         35738,      35400,      35061,      34721,      34379,      34036, 
         33692,      33346,      33000,      32652,      32302,      31952, 
         31600,      31247,      30893,      30538,      30182,      29824, 
         29465,      29106,      28745,      28383,      28020,      27656, 
         27291,      26925,      26558,      26189,      25820,      25450, 
         25079,      24707,      24334,      23961,      23586,      23210, 
         22834,      22456,      22078,      21699,      21319,      20939, 
         20557,      20175,      19792,      19408,      19024,      18639, 
         18253,      17866,      17479,      17091,      16703,      16313, 
         15924,      15533,      15142,      14751,      14359,      13966, 
         13573,      13179,      12785,      12390,      11995,      11600, 
         11204,      10807,      10411,      10013,       9616,       9218, 
          8820,       8421,       8022,       7623,       7223,       6823, 
          6423,       6023,       5622,       5222,       4821,       4420, 
          4018,       3617,       3215,       2814,       2412,       2010, 
          1608,       1206,        804,        402,          0, 4294966894, 
    4294966492, 4294966090, 4294965688, 4294965286, 4294964884, 4294964483, 
    4294964081, 4294963679, 4294963278, 4294962877, 4294962475, 4294962074, 
    4294961674, 4294961273, 4294960873, 4294960473, 4294960073, 4294959673, 
    4294959274, 4294958875, 4294958477, 4294958078, 4294957680, 4294957283, 
    4294956886, 4294956489, 4294956092, 4294955696, 4294955301, 4294954906, 
    4294954511, 4294954117, 4294953723, 4294953330, 4294952938, 4294952545, 
    4294952154, 4294951763, 4294951373, 4294950983, 4294950594, 4294950205, 
    4294949817, 4294949430, 4294949043, 4294948658, 4294948272, 4294947888, 
    4294947504, 4294947121, 4294946739, 4294946358, 4294945977, 4294945597, 
    4294945218, 4294944840, 4294944463, 4294944086, 4294943710, 4294943336, 
    4294942962, 4294942589, 4294942217, 4294941846, 4294941476, 4294941107, 
    4294940739, 4294940371, 4294940005, 4294939640, 4294939276, 4294938913, 
    4294938551, 4294938191, 4294937831, 4294937472, 4294937115, 4294936758, 
    4294936403, 4294936049, 4294935696, 4294935344, 4294934994, 4294934645, 
    4294934297, 4294933950, 4294933604, 4294933260, 4294932917, 4294932575, 
    4294932235, 4294931896, 4294931558, 4294931222, 4294930887, 4294930553, 
    4294930221, 4294929890, 4294929560, 4294929232, 4294928906, 4294928580, 
    4294928257, 4294927935, 4294927614, 4294927294, 4294926977, 4294926660, 
    4294926346, 4294926033, 4294925721, 4294925411, 4294925102, 4294924795, 
    4294924490, 4294924187, 4294923884, 4294923584, 4294923285, 4294922988, 
    4294922693, 4294922399, 4294922107, 4294921816, 4294921528, 4294921241, 
    4294920956, 4294920672, 4294920390, 4294920110, 4294919832, 4294919556, 
    4294919281, 4294919009, 4294918738, 4294918468, 4294918201, 4294917936, 
    4294917672, 4294917410, 4294917151, 4294916893, 4294916637, 4294916382, 
    4294916130, 4294915880, 4294915632, 4294915385, 4294915141, 4294914898, 
    4294914658, 4294914419, 4294914182, 4294913948, 4294913715, 4294913485, 
    4294913256, 4294913030, 4294912805, 4294912583, 4294912363, 4294912144, 
    4294911928, 4294911714, 4294911502, 4294911292, 4294911085, 4294910879, 
    4294910675, 4294910474, 4294910275, 4294910078, 4294909883, 4294909690, 
    4294909499, 4294909311, 4294909124, 4294908940, 4294908758, 4294908579, 
    4294908401, 4294908226, 4294908053, 4294907882, 4294907713, 4294907547, 
    4294907383, 4294907221, 4294907062, 4294906904, 4294906749, 4294906597, 
    4294906446, 4294906298, 4294906152, 4294906009, 4294905867, 4294905728, 
    4294905592, 4294905457, 4294905325, 4294905196, 4294905068, 4294904944, 
    4294904821, 4294904701, 4294904583, 4294904467, 4294904354, 4294904243, 
    4294904135, 4294904029, 4294903925, 4294903824, 4294903725, 4294903628, 
    4294903534, 4294903443, 4294903353, 4294903266, 4294903182, 4294903100, 
    4294903020, 4294902943, 4294902868, 4294902796, 4294902726, 4294902658, 
    4294902593, 4294902530, 4294902470, 4294902412, 4294902357, 4294902304, 
    4294902254, 4294902206, 4294902160, 4294902117, 4294902076, 4294902038, 
    4294902003, 4294901969, 4294901939, 4294901910, 4294901884, 4294901861, 
    4294901840, 4294901821, 4294901805, 4294901792, 4294901781, 4294901772, 
    4294901766, 4294901762, 4294901761, 4294901762, 4294901766, 4294901772, 
    4294901781, 4294901792, 4294901805, 4294901821, 4294901840, 4294901861, 
    4294901884, 4294901910, 4294901939, 4294901969, 4294902003, 4294902038, 
    4294902077, 4294902117, 4294902160, 4294902206, 4294902254, 4294902304, 
    4294902357, 4294902413, 4294902470, 4294902531, 4294902593, 4294902658, 
    4294902726, 4294902796, 4294902868, 4294902943, 4294903020, 4294903100, 
    4294903182, 4294903267, 4294903354, 4294903443, 4294903535, 4294903629, 
    4294903725, 4294903824, 4294903925, 4294904029, 4294904135, 4294904244, 
    4294904354, 4294904468, 4294904583, 4294904701, 4294904821, 4294904944, 
    4294905069, 4294905196, 4294905326, 4294905458, 4294905592, 4294905729, 
    4294905868, 4294906009, 4294906153, 4294906299, 4294906447, 4294906597, 
    4294906750, 4294906905, 4294907062, 4294907222, 4294907384, 4294907548, 
    4294907714, 4294907883, 4294908054, 4294908227, 4294908402, 4294908579, 
    4294908759, 4294908941, 4294909125, 4294909311, 4294909500, 4294909691, 
    4294909883, 4294910078, 4294910275, 4294910475, 4294910676, 4294910880, 
    4294911085, 4294911293, 4294911503, 4294911715, 4294911929, 4294912145, 
    4294912364, 4294912584, 4294912806, 4294913031, 4294913257, 4294913486, 
    4294913716, 4294913949, 4294914183, 4294914420, 4294914659, 4294914899, 
    4294915142, 4294915386, 4294915633, 4294915881, 4294916131, 4294916383, 
    4294916638, 4294916894, 4294917152, 4294917412, 4294917673, 4294917937, 
    4294918202, 4294918470, 4294918739, 4294919010, 4294919282, 4294919557, 
    4294919833, 4294920112, 4294920392, 4294920673, 4294920957, 4294921242, 
    4294921529, 4294921818, 4294922108, 4294922400, 4294922694, 4294922989, 
    4294923286, 4294923585, 4294923886, 4294924188, 4294924491, 4294924797, 
    4294925104, 4294925412, 4294925722, 4294926034, 4294926347, 4294926662, 
    4294926978, 4294927296, 4294927615, 4294927936, 4294928258, 4294928582, 
    4294928907, 4294929234, 4294929562, 4294929891, 4294930222, 4294930554, 
    4294930888, 4294931223, 4294931560, 4294931897, 4294932236, 4294932577, 
    4294932918, 4294933261, 4294933606, 4294933951, 4294934298, 4294934646, 
    4294934995, 4294935346, 4294935698, 4294936050, 4294936405, 4294936760, 
    4294937116, 4294937474, 4294937832, 4294938192, 4294938553, 4294938915, 
    4294939278, 4294939642, 4294940007, 4294940373, 4294940740, 4294941108, 
    4294941477, 4294941847, 4294942219, 4294942591, 4294942963, 4294943337, 
    4294943712, 4294944088, 4294944464, 4294944841, 4294945220, 4294945599, 
    4294945979, 4294946359, 4294946741, 4294947123, 4294947506, 4294947890, 
    4294948274, 4294948659, 4294949045, 4294949432, 4294949819, 4294950207, 
    4294950595, 4294950984, 4294951374, 4294951765, 4294952156, 4294952547, 
    4294952939, 4294953332, 4294953725, 4294954119, 4294954513, 4294954907, 
    4294955302, 4294955698, 4294956094, 4294956490, 4294956887, 4294957284, 
    4294957682, 4294958080, 4294958478, 4294958877, 4294959276, 4294959675, 
    4294960075, 4294960475, 4294960875, 4294961275, 4294961675, 4294962076, 
    4294962477, 4294962878, 4294963280, 4294963681, 4294964083, 4294964484, 
    4294964886, 4294965288, 4294965690, 4294966092, 4294966494, 4294966896, 
             1,        403,        805,       1208,       1610,       2012, 
          2413,       2815,       3217,       3619,       4020,       4421, 
          4822,       5223,       5624,       6025,       6425,       6825, 
          7225,       7624,       8024,       8423,       8821,       9219, 
          9617,      10015,      10412,      10809,      11205,      11601, 
         11997,      12392,      12787,      13181,      13575,      13968, 
         14360,      14752,      15144,      15535,      15925,      16315, 
         16704,      17093,      17481,      17868,      18254,      18640, 
         19025,      19410,      19794,      20177,      20559,      20940, 
         21321,      21701,      22080,      22458,      22835,      23212, 
         23587,      23962,      24336,      24709,      25081,      25452, 
         25822,      26191,      26559,      26926,      27292,      27657, 
         28021,      28384,      28746,      29107,      29467,      29826, 
         30183,      30539,      30895,      31249,      31602,      31953, 
         32304,      32653,      33001,      33348,      33693,      34038, 
         34381,      34722,      35063,      35402,      35740,      36076, 
         36411,      36745,      37077,      37408,      37737,      38065, 
         38392,      38717,      39041,      39363,      39684,      40003, 
         40321,      40637,      40952,      41265,      41577,      41887, 
         42195,      42502,      42807,      43111,      43413,      43714, 
         44012,      44309,      44605,      44899,      45191,      45481, 
         45770,      46057,      46342,      46625,      46907,      47187, 
         47465,      47742,      48016,      48289,      48560,      48829, 
         49096,      49362,      49625,      49887,      50147,      50405, 
         50661,      50915,      51167,      51418,      51666,      51912, 
         52157,      52399,      52640,      52878,      53115,      53350, 
         53582,      53813,      54041,      54268,      54492,      54714, 
         54935,      55153,      55369,      55583,      55795,      56005, 
         56213,      56419,      56622,      56824,      57023,      57220, 
         57415,      57608,      57798,      57987,      58173,      58357, 
         58539,      58719,      58896,      59071,      59245,      59415, 
         59584,      59750,      59914,      60076,      60236,      60393, 
         60548,      60701,      60851,      60999,      61145,      61289, 
         61430,      61569,      61706,      61840,      61972,      62101, 
         62229,      62354,      62476,      62597,      62715,      62830, 
         62943,      63054,      63162,      63269,      63372,      63473, 
         63572,      63669,      63763,      63855,      63944,      64031, 
         64115,      64197,      64277,      64354,      64429,      64501, 
         64571,      64639,      64704,      64767,      64827,      64885, 
         64940,      64993,      65043,      65091,      65137,      65180, 
         65221,      65259,      65294,      65328,      65359,      65387, 
         65413,      65436,      65457,      65476,      65492,      65505, 
         65516,      65525,      65531,      65535 };