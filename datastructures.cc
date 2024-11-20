/**
 * @brief The Datastructures class
 * STUDENTS: Modify the code below to implement the functionality of the class.
 * Also remove comments from the parameter names when you implement an operation
 * (Commenting out parameter name prevents compiler from warning about unused
 * parameters on operations you haven't yet implemented.)
 */

#include "datastructures.hh"
#include "customtypes.hh"

Datastructures::Datastructures()
{

}

Datastructures::~Datastructures()
{

}

unsigned int Datastructures::get_bite_count()
{
    return bites.size();
}

void Datastructures::clear_all()
{
    bites.clear();
    contours.clear();
    bite_ids.clear();
    coord_to_bite.clear();
    alphabetical_bites.clear();
    distance_sorted_bites.clear();

}

std::vector<BiteID> Datastructures::all_bites()
{
    if (alphabetical_bites.size() == get_bite_count()) {
            return alphabetical_bites;
        }
        std::vector<BiteID> collected_bites;
        collected_bites.reserve(get_bite_count());

        for (const auto& pair : bites) {
            collected_bites.push_back(pair.first);
        }

    return collected_bites;
}

bool Datastructures::add_bite(BiteID id, const Name & name, Coord xy)
{
    //Checks whether the bite with given id exists or a bite already exists at the given coordinates
    if (bites.find(id) != bites.end() || coord_to_bite.find(xy) != coord_to_bite.end()) {
            return false;
        }

    // Adding the bite
    bites.emplace(id, Bite{name, xy, NO_CONTOUR});
    coord_to_bite.emplace(xy, id);
    bite_ids.push_back(id);

    // Find the correct position to insert the new bite in the sorted vector
    auto it = std::lower_bound(alphabetical_bites.begin(), alphabetical_bites.end(), id,
                                   [this](BiteID a, BiteID b) {
                                       if (bites[a].name == bites[b].name) {
                                           return a < b; // Sort by id if names are the same
                                       }
                                       return bites[a].name < bites[b].name; // Sort by name
                                   });

        // Insert at the correct position to maintain sorted order
        alphabetical_bites.insert(it, id);



        auto dist_it = std::lower_bound(distance_sorted_bites.begin(), distance_sorted_bites.end(), id,
                                             [this, &xy](BiteID a, BiteID b) {
                                                 // Calculate distance for bite a
                                                 int dist_a = std::abs(bites[a].coord.x) + std::abs(bites[a].coord.y);
                                                 // Calculate distance for bite b
                                                 int dist_b = std::abs(bites[b].coord.x) + std::abs(bites[b].coord.y);

                                                 // Calculate distance for the new bite
                                                 int new_distance = std::abs(xy.x) + std::abs(xy.y);

                                                 // Compare by distance, then by y-coordinate, then by ID
                                                 if (new_distance != dist_a && new_distance != dist_b) {
                                                     return new_distance < dist_a; // Comparison against existing bite distances
                                                 } else if (dist_a != dist_b) {
                                                     return dist_a < dist_b;
                                                 } else if (bites[a].coord.y != bites[b].coord.y) {
                                                     return bites[a].coord.y < bites[b].coord.y;
                                                 }
                                                 return a < b; // Sort by id if distances and y-coordinates are the same
                                             });

        // Insert at the correct position to maintain sorted order
        distance_sorted_bites.insert(dist_it, id);


    return true;
}

Name Datastructures::get_bite_name(BiteID id)
{
    // Find a bite with a given id
    auto it = bites.find(id);
    return (it != bites.end()) ? it->second.name : NO_NAME;

}

Coord Datastructures::get_bite_coord(BiteID id)
{
    //Find the cordinates of a given bite.
    auto it = bites.find(id);
    return (it != bites.end()) ? it->second.coord : NO_COORD;
}

std::vector<BiteID> Datastructures::get_bites_alphabetically()
{
    if (asorted_) {return alphabetical_bites;}
    std::vector<BiteID> testi = all_bites();
    std::vector<BiteID> testi2 = all_bites();
    asorted_ = true;
    return alphabetical_bites;

}

std::vector<BiteID> Datastructures::get_bites_distance_increasing()
{
    if (dsorted_) {return distance_sorted_bites;}
    std::vector<BiteID> testi = all_bites();
    std::vector<BiteID> testi2 = all_bites();
    dsorted_ = true;
    return distance_sorted_bites;
}





BiteID Datastructures::find_bite_with_coord(Coord xy)
{
    auto it = coord_to_bite.find(xy);
    if (it != coord_to_bite.end()) {
        return it->second;
    }
    return NO_BITE;
}

bool Datastructures::change_bite_coord(BiteID id, Coord newcoord)
{
    // Checks whether the bite exists
    auto biteIt = bites.find(id);
    if (biteIt == bites.end()) {
        return false;
    }

    //If bite already exists
    if (coord_to_bite.find(newcoord) != coord_to_bite.end()) {
        return false;
    }

    ContourID contour_id = biteIt->second.contour_id;
        if (contour_id != NO_CONTOUR) {
            auto contourIt = contours.find(contour_id);
            if (contourIt != contours.end()) {
                const std::vector<Coord>& contour_coords = contourIt->second.coords;
                std::unordered_set<Coord> coords_set(contour_coords.begin(), contour_coords.end());
                if (coords_set.find(newcoord) == coords_set.end()) {
                    // New coordinate is not part of the contour
                    return false;
                }
            }
        }
        coord_to_bite.erase(biteIt->second.coord);  // Remove the old coordinate and adding the new one
        coord_to_bite[newcoord] = id;

        //Update done in the bites map
        biteIt->second.coord = newcoord;
        return true;

}

bool Datastructures::add_contour(ContourID id, const Name & name, ContourHeight height, std::vector<Coord> coords)
{


    // Check if the contour ID is unique
        if (contours.find(id) != contours.end()) {
            return false;
        }

        // Check if the height is within the valid range
        if (height > MAX_CONTOUR_HEIGHT || height < -MAX_CONTOUR_HEIGHT) {
            return false;
        }

        // Check if the coordinates list is empty
        if (coords.empty()) {
            return false;
        }

        // If all checks passed, add the contour to the data structure
        contours[id] = {name, height, coords, NO_CONTOUR, {}};

        return true;
}

std::vector<ContourID> Datastructures::all_contours()
{

    std::vector<ContourID> ids; // Initializing a Vector

    // Iterate over the contours map and collect keys
    for (const auto& [id, _] : contours) {
        ids.push_back(id);
    }

    return ids; // Return the vector of IDs

}

Name Datastructures::get_contour_name(ContourID id)
{
    auto it = contours.find(id); // find the contour by ID
    if (it != contours.end()) {
        return it->second.name;
    } else {
        return NO_NAME;
    }
}

std::vector<Coord> Datastructures::get_contour_coords(ContourID id)
{

    auto it = contours.find(id); // find the contour by ID

    if (it != contours.end()) {
        return it->second.coords;
    } else {
        return std::vector<Coord>{NO_COORD};
    }
}

ContourHeight Datastructures::get_contour_height(ContourID id)
{


    auto it = contours.find(id); // Find the contour by ID

    if (it != contours.end()) {
        return it->second.height;
    } else {
        return NO_CONTOUR_HEIGHT;
    }
}

bool Datastructures::add_subcontour_to_contour(ContourID childId,
                                               ContourID parentId)
{

    auto parentIt = contours.find(parentId); // Find the parent contour
    auto childIt = contours.find(childId); // Find the child contour

    // Check if both contours exist
    if (parentIt == contours.end() || childIt == contours.end()) {
        return false;
    }

    // Check if the child contour already has a parent
    if (childIt->second.parent_id != NO_CONTOUR) {
        return false;
    }

    // heights
    ContourHeight parentHeight = parentIt->second.height;
    ContourHeight childHeight = childIt->second.height;

    // Check conditions
    if (abs(childHeight) <= abs(parentHeight)) {
        return false;
    }
    if (std::abs(childHeight - parentHeight) != 1) {

        return false;
    }

    parentIt->second.sub_contours.push_back(childId);
    childIt->second.parent_id = parentId;

    return true;
}

bool Datastructures::add_bite_to_contour(BiteID biteId, ContourID contourId)
{

    // Check if the contour exists
    auto contourIt = contours.find(contourId);
    if (contourIt == contours.end()) {
        return false;
    }

    // Check if the bite exists
    auto biteIt = bites.find(biteId);
    if (biteIt == bites.end()) {
        return false;
    }

    // Check if the bite already belongs to another contour
    if (biteIt->second.contour_id != NO_CONTOUR) {
        return false;
    }

    // Check if the bite coordinates are part of the specified contour
    const auto& contourCoords = contourIt->second.coords;
    const Coord& biteCoord = biteIt->second.coord;

    // Check if the bite coordinates match any coordinates in the contour
    if (std::find(contourCoords.begin(), contourCoords.end(), biteCoord) == contourCoords.end()) {
        return false;
    }


    //Else
    contourIt->second.sub_contours.push_back(biteId);
    biteIt->second.contour_id = contourId; // Update bite's contour_id
    return true;

}

std::vector<ContourID> Datastructures::get_bite_in_contours(BiteID id)
{

    // Check if the bite exists in the data structure
        auto biteIt = bites.find(id);
        if (biteIt == bites.end()) {

            return {NO_CONTOUR};
        }


        ContourID current_contour = biteIt->second.contour_id;
        if (current_contour == NO_CONTOUR) {

            return {};
        }

        // adding contour IDs to the result
        std::vector<ContourID> contour_chain;
        while (current_contour != NO_CONTOUR) {
            contour_chain.push_back(current_contour);
            auto contourIt = contours.find(current_contour);

            // if we can't find the parent contour, break the loop
            if (contourIt == contours.end()) {
                break;
            }


            current_contour = contourIt->second.parent_id;
        }

        return contour_chain;
}

std::vector<ContourID>
Datastructures::all_subcontours_of_contour(ContourID id)
{
    // If the contour with the given ID does not exist
    if (contours.find(id) == contours.end()) {
        return {NO_CONTOUR};
    }

    // to store all sub-contours
    std::vector<ContourID> subcontours;

    // Helper function
    std::function<void(ContourID)> collect_subcontours = [&](ContourID current_id) {
        // current contour
        const Contour& current_contour = contours[current_id];


        for (ContourID sub_id : current_contour.sub_contours) {
            subcontours.push_back(sub_id);
            collect_subcontours(sub_id);
        }
    };

    collect_subcontours(id);

    return subcontours;


}

ContourID
Datastructures::get_closest_common_ancestor_of_contours(ContourID id1,
                                                        ContourID id2)
{
    // Find the contours with the given IDs
        auto it1 = contours.find(id1);
        auto it2 = contours.find(id2);
        if (it1 == contours.end() || it2 == contours.end()) {
            return NO_CONTOUR;
        }

        // Helper function
        auto get_ancestors = [&](ContourID id) -> std::vector<ContourID> {
            std::vector<ContourID> ancestors;
            ContourID current = id;
            while (true) {
                auto it = contours.find(current);
                if (it == contours.end() || it->second.parent_id == NO_CONTOUR) {
                    break;
                }
                current = it->second.parent_id;
                ancestors.push_back(current);
            }
            return ancestors;
        };

        // Get the ancestors of both contours
        std::vector<ContourID> ancestors1 = get_ancestors(id1);
        std::vector<ContourID> ancestors2 = get_ancestors(id2);

        if (ancestors1.empty() || ancestors2.empty()) {
            return NO_CONTOUR;
        }

        // closest to farthest
        for (auto ancestor1 : ancestors1) {
            for (auto ancestor2 : ancestors2) {
                if (ancestor1 == ancestor2) {
                    return ancestor1;
                }
            }
        }

        // If no common ancestor is found
        return NO_CONTOUR;
}

bool Datastructures::remove_bite(BiteID id)
{

    // Check if the bite exists in the map
       auto it = bites.find(id);
       if (it == bites.end()) {
           return false;  // Bite does not exist, return false
       }

       // Get the corresponding coordinate of the bite
       Coord coord = it->second.coord;

       // Remove from the bites map
       bites.erase(it);

       // Remove from the coord_to_bite map
       coord_to_bite.erase(coord);

       // Remove from the bite_ids vector
       bite_ids.erase(std::remove(bite_ids.begin(), bite_ids.end(), id), bite_ids.end());

       // Remove from the alphabetical_bites vector
       alphabetical_bites.erase(std::remove(alphabetical_bites.begin(), alphabetical_bites.end(), id), alphabetical_bites.end());

       // Remove from the distance_sorted_bites vector
       distance_sorted_bites.erase(std::remove(distance_sorted_bites.begin(), distance_sorted_bites.end(), id), distance_sorted_bites.end());

       return true;  // Bite was successfully removed
}

std::vector<BiteID> Datastructures::get_bites_closest_to(Coord xy)
{

    // If there are no bites, return an empty vector
        if (bites.empty()) {
            return {};
        }

        // Intialize a vector to store distances
        std::vector<std::tuple<BiteID, int, Coord>> bite_distances;

        // Manhattan distance
        for (const auto& [id, bite] : bites) {
            int distance = std::abs(bite.coord.x - xy.x) + std::abs(bite.coord.y - xy.y);
            bite_distances.push_back({id, distance, bite.coord});
        }

        // Sort the bites
        std::sort(bite_distances.begin(), bite_distances.end(),
            [](const std::tuple<BiteID, int, Coord>& a, const std::tuple<BiteID, int, Coord>& b) {
                int dist_a = std::get<1>(a);
                int dist_b = std::get<1>(b);

                if (dist_a != dist_b) {
                    return dist_a < dist_b;  //by distance first
                }

                Coord coord_a = std::get<2>(a);
                Coord coord_b = std::get<2>(b);

                if (coord_a.y != coord_b.y) {
                    return coord_a.y < coord_b.y;  // by y-coordinate if distances are the same
                }

                return std::get<0>(a) < std::get<0>(b);  // by BiteID if y-coordinates are the same
            }
        );

        // Extract the IDs of up to three closest bites
        std::vector<BiteID> closest_bites;
        for (size_t i = 0; i < std::min<size_t>(3, bite_distances.size()); ++i) {
            closest_bites.push_back(std::get<0>(bite_distances[i]));
        }

        return closest_bites;
}
