async function fetchDoctors() {
  try {
    const response = await fetch("/api/doctors");
    const data = await response.json();
    console.log("Doctors data:", JSON.stringify(data));
    displayDoctors(data.assets);
  } catch (error) {
    console.error("Error fetching doctors:", error);
  }
}

function displayDoctors(assets) {
  const floors = {};

  // Group doctors by floor
  assets.forEach((asset) => {
    if (!floors[asset.floor]) {
      floors[asset.floor] = [];
    }
    floors[asset.floor].push(asset);
  });

  const floorsContainer = document.getElementById("floors");
  floorsContainer.innerHTML = ""; // Clear any existing content

  // Create floor sections dynamically
  for (let floor in floors) {
    const floorSection = document.createElement("div");
    floorSection.classList.add("floor");

    const floorTitle = document.createElement("h2");
    floorTitle.innerText = `Floor ${floor}`;
    floorSection.appendChild(floorTitle);

    // Display doctors for the floor
    floors[floor].forEach((asset) => {
      const assetCard = document.createElement("div");
      assetCard.classList.add("doctor-card");

      const assetName = document.createElement("h3");
      assetName.innerText = asset.name;
      assetCard.appendChild(assetName);

      if (asset.name != null) {
        const assetImage = document.createElement("img");
        assetImage.classList.add("doctor-image");
        switch (asset.type) {
          case "doctor":
            assetImage.src = "images/doctor_img.png";
            break;
          case "equipment":
            assetImage.src = "images/equipment_img.png";
            break;
          case "empty":
            assetImage.style.display = "none";
            assetCard.style.width = "0px";
            break;
          case "lost":
            assetImage.src = "images/lost.png";
            assetName.style.color = "red";
            break;
          default:
            assetImage.style.display = "none";
            assetCard.style.width = "0px";
            break;
        }
        assetImage.alt = `${asset.name}`;
        assetImage.style.width = "50px";
        assetImage.style.height = "50px";
        assetCard.appendChild(assetImage);
      }

      floorSection.appendChild(assetCard);
    });

    floorsContainer.appendChild(floorSection);
  }
}

fetchDoctors();
setInterval(fetchDoctors, 5000);
