/*PGR-GNU*****************************************************************
Copyright (c) 2016 Andrea Nardelli
Mail: nrd.nardelli@gmail.com
------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

DROP TABLE IF EXISTS flow_example;
CREATE TABLE flow_example (
	id SERIAL,
	source INTEGER,
	target INTEGER,
	capacity INTEGER
	);

INSERT INTO flow_example (source, target, capacity) VALUES
(1,2,10), (1,3,10),
(2,3,2), (2,4,4),
(2,5,8), (3,5,9),
(4,6,10), (5,4,6),(5,6,10);


SELECT * FROM pgr_edmondsKarp(
    'SELECT id, source, target, capacity FROM flow_example',
     1, 6
);

/********************** RESULTS
seq | edge | start_vid | end_vid | flow | residual_capacity 
-----+------+-----------+---------+------+-------------------
   1 |    1 |         1 |       2 |   10 |                 0
   2 |    2 |         1 |       3 |    9 |                 1
   3 |    4 |         2 |       4 |    4 |                 0
   4 |    5 |         2 |       5 |    6 |                 2
   5 |    6 |         3 |       5 |    9 |                 0
   6 |    7 |         4 |       6 |    9 |                 1
   7 |    8 |         5 |       4 |    5 |                 1
   8 |    9 |         5 |       6 |   10 |                 0
(8 rows)

**********************/
