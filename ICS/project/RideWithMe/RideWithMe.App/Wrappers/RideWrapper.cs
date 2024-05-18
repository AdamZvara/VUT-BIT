using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using RideWithMe.App.Extensions;
using RideWithMe.BL.Models;

namespace RideWithMe.App.Wrappers;

public class RideWrapper : ModelWrapper<RideDetailModel>
{
    public RideWrapper(RideDetailModel model) : base(model)
    {
        InitializeCollectionProperties(model);
    }
    public Guid? DriverId
    {
        get => GetValue<Guid>();
        set => SetValue(value);
    }
    public Guid? CarId
    {
        get => GetValue<Guid>();
        set => SetValue(value);
    }
    public Guid? StartLocationId
    {
        get => GetValue<Guid>();
        set => SetValue(value);
    }
    public Guid? EndLocationId
    {
        get => GetValue<Guid>();
        set => SetValue(value);
    }

    public DateTime StartTime
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }

    public DateTime EndTime
    {
        get => GetValue<DateTime>();
        set => SetValue(value);
    }

    public RideDetailCarModel? Car
    {
        get => GetValue<RideDetailCarModel>();
        set => SetValue(value);
    }
    public RideDetailDriverModel? Driver
    {
        get => GetValue<RideDetailDriverModel>();
        set => SetValue(value);
    }

    public RideDetailLocationModel? StartLocation
    {
        get => GetValue<RideDetailLocationModel>();
        set => SetValue(value);
    }
    public RideDetailLocationModel? EndLocation
    {
        get => GetValue<RideDetailLocationModel>();
        set => SetValue(value);
    }
    public ObservableCollection<UserWrapper> Passengers { get; set; } = new();

    private void InitializeCollectionProperties(RideDetailModel model)
    {
        if (model.Passengers == null)
        {
            throw new ArgumentException("Ingredients cannot be null");
        }
        Passengers.AddRange(model.Passengers.Select(e => new UserWrapper(e)));

        RegisterCollection(Passengers, model.Passengers);
    }

    public override IEnumerable<ValidationResult> Validate(ValidationContext validationContext)
    {
        if (CarId == Guid.Empty)
        {
            yield return new ValidationResult($"{nameof(CarId)} is required", new[] { nameof(CarId) });
        }

        if (StartLocationId == Guid.Empty)
        {
            yield return new ValidationResult($"{nameof(StartLocationId)} is required", new[] { nameof(StartLocationId) });
        }

        if (EndLocationId == Guid.Empty)
        {
            yield return new ValidationResult($"{nameof(StartLocationId)} is required", new[] { nameof(StartLocationId) });
        }
    }

        public static implicit operator RideWrapper(RideDetailModel userModel) => new(userModel);

        public static implicit operator RideDetailModel(RideWrapper wrapper) => wrapper.Model;
}