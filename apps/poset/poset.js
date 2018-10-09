"use script";

function draw_poset(poset, selector = "svg") {
  var svg = d3.select(selector),
    width = svg.node().getBoundingClientRect().width,
    height = svg.node().getBoundingClientRect().height;

  var link = svg.append("g")
    .attr("class", "edge")
    .selectAll("line")
    .data(poset.edges)
    .enter().append("line");

  var node = svg.append("g")
    .attr("class", "node")
    .selectAll("circle")
    .data(poset.nodes)
    .enter().append("circle")
    .attr("r", 3)
    .call(d3.drag()
      .on("start", drag_started)
      .on("drag", drag_moved)
      .on("end", drag_ended));

  node.append("title")
    .text(function (d) { return d.id; });

  var simulation = d3.forceSimulation()
    .force("link", d3.forceLink().id(function (d) { return d.id; }))
    .force("charge", d3.forceManyBody())
    .force("center", d3.forceCenter(width / 2, height / 2))
    .force("order", d3.forceY(function (d) { return -d.height * 30; }).strength(0.1));

  simulation
    .nodes(poset.nodes)
    .on("tick", update_poset);

  simulation.force("link")
    .links(poset.edges);

  function update_poset() {
    link
      .attr("x1", function (d) { return d.source.x; })
      .attr("y1", function (d) { return d.source.y; })
      .attr("x2", function (d) { return d.target.x; })
      .attr("y2", function (d) { return d.target.y; });

    node
      .attr("cx", function (d) { return d.x; })
      .attr("cy", function (d) { return d.y; });
  }

  function drag_started(d) {
    if (!d3.event.active) {
      simulation.alphaTarget(0.3).restart();
    }
    d.fx = d.x;
    d.fy = d.y;
  }

  function drag_moved(d) {
    d.fx = d3.event.x;
    d.fy = d3.event.y;
  }

  function drag_ended(d) {
    if (!d3.event.active) {
      simulation.alphaTarget(0);
    }
    d.fx = null;
    d.fy = null;
  }
};
